/*
  This file is part of t8code.
  t8code is a C library to manage a collection (a forest) of multiple
  connected adaptive space-trees of general element classes in parallel.

  Copyright (C) 2015 the developers

  t8code is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  t8code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with t8code; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <gtest/gtest.h>
#include <test/t8_gtest_macros.hxx>
#include <t8_cmesh.h>
#include <t8_schemes/t8_default/t8_default_cxx.hxx>
#include "t8_cmesh/t8_cmesh_trees.h"
#include "t8_cmesh/t8_cmesh_partition.h"
#include <t8_cmesh/t8_cmesh_testcases.h>

/* At the time of this writing (November 15 2023) t8_cmesh_offset_concentrate
 * has a comment stating it does not work with non-derived cmeshes.
 * We write the tests in this file to check this.
 * 
 * Theses tests will create an offset array, build a new cmesh
 * with trees of the same eclass, set the offset array and commit the cmesh.
 * We will iterate through all eclasses and all tree counts up to a given maximum.
 */

/* The maximum number of trees for a cmesh to test with.
 * We will test all numbers of trees from 0 to the maximum. */
#define T8_TEST_PARTITION_OFFSET_MAX_TREE_NUM 100

class cmesh_set_partition_offsets: public testing::TestWithParam<std::tuple<t8_eclass, int>> {
 protected:
  void
  SetUp () override
  {
    ieclass = std::get<0> (GetParam ());
    inum_trees = std::get<1> (GetParam ());
  }
  t8_eclass_t ieclass;
  t8_gloidx_t inum_trees;
};

/* call t8_cmesh_offset_concentrate for non-derived cmesh 
 * and destroy it before commit. */
TEST_P (cmesh_set_partition_offsets, test_set_offsets_nocommit)
{
  t8_cmesh_t cmesh;
  const int main_process = 0;
  /* Build a valid offset array. For this test it is onlt necessary that 
   * the array corresponds to any valid partition.
   * We use the offset_concentrate function to build an offset array for a partition
   * that concentrates all trees at one process. */
  t8_shmem_init (sc_MPI_COMM_WORLD);
  t8_shmem_array_t shmem_array = t8_cmesh_offset_concentrate (main_process, sc_MPI_COMM_WORLD, inum_trees);

  /* Initialize the cmesh */
  t8_cmesh_init (&cmesh);

  /* Set the partition offsets */
  t8_cmesh_set_partition_offsets (cmesh, shmem_array);
  /* Destroy the cmesh */
  t8_cmesh_unref (&cmesh);
}

/* call t8_cmesh_offset_concentrate for non-derived cmesh 
 * and commit it. */
TEST_P (cmesh_set_partition_offsets, test_set_offsets_commit)
{
  t8_cmesh_t cmesh;
  const int main_process = 0;
  sc_MPI_Comm comm = sc_MPI_COMM_WORLD;
  /* Build a valid offset array. For this test it is only necessary that 
   * the array corresponds to any valid partition.
   * We use the offset_concentrate function to build an offset array for a partition
   * that concentrates all trees at one process. */
  t8_shmem_init (comm);
  t8_shmem_array_t shmem_array = t8_cmesh_offset_concentrate (main_process, comm, inum_trees);

  /* Initialize the cmesh */
  t8_cmesh_init (&cmesh);

  /* Set the partition offsets */
  t8_cmesh_set_partition_offsets (cmesh, shmem_array);

  /* Specify a dimension */
  const int dim = t8_eclass_to_dimension[ieclass];
  t8_cmesh_set_dimension (cmesh, dim);

  /* Set class for the trees */
  for (int itree = 0; itree < inum_trees; ++itree) {
    t8_cmesh_set_tree_class (cmesh, itree, ieclass);
  }

  /* Commit the cmesh */
  t8_cmesh_commit (cmesh, comm);

  /* Check that the cmesh was partitioned according to the offset */

  ASSERT_TRUE (t8_cmesh_is_committed (cmesh));

  /* Get the mpirank */
  int mpirank;
  int mpiret;

  mpiret = sc_MPI_Comm_rank (comm, &mpirank);
  SC_CHECK_MPI (mpiret);

  /* Get the local number of trees */
  const t8_locidx_t num_local_trees = t8_cmesh_get_num_local_trees (cmesh);
  /* Compute the reference value, num_trees for mpirank main_process,
   * 0 on each other rank. */
  const t8_locidx_t expected_num_local_trees = mpirank == main_process ? inum_trees : 0;

  EXPECT_EQ (num_local_trees, expected_num_local_trees);
  EXPECT_EQ (t8_cmesh_get_num_trees (cmesh), inum_trees);

  /* Destroy the cmesh */
  t8_cmesh_unref (&cmesh);
}

/* Make atest suite that iterates over all classes and a tree count from 0 to the maximum. */
INSTANTIATE_TEST_SUITE_P (t8_cmesh_set_partition_offsets, cmesh_set_partition_offsets,
                          testing::Combine (AllEclasses, testing::Values (0, T8_TEST_PARTITION_OFFSET_MAX_TREE_NUM)));
