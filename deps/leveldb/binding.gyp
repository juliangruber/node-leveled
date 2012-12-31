# Copyright (c) 2011 The LevelDB Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

{
  'variables': {
    'use_snappy%': 0,
  },
  'target_defaults': {
    'defines': [
      'LEVELDB_PLATFORM_POSIX=1'
    ],
    'include_dirs': [
      '.',
      'include/',
    ],
    'conditions': [
      ['OS == "win"', {
        'include_dirs': [
          'port/win',
        ],
      }],
      ['OS == "linux"', {
        'defines': ['OS_LINUX=1']  
      }],
      ['OS == "mac"', {
        'defines': ['OS_MACOSX=1']  
      }],
      ['use_snappy', {
        'defines': [
          'USE_SNAPPY=1',
        ],
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'leveldb',
      'type': 'static_library',
      'conditions': [
        ['use_snappy', {
          'dependencies': [
            '../../third_party/snappy/snappy.gyp:snappy',
          ],
        }],
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'include/',
        ],
        'conditions': [
          ['OS == "win"', {
            'include_dirs': [
              'port/win',
            ],
          }],
        ],
      },
      'sources': [
        # Include and then exclude so that all files show up in IDEs, even if
        # they don't build.
        'db/builder.cc',
        'db/builder.h',
        'db/c.cc',
        'db/dbformat.cc',
        'db/dbformat.h',
        'db/db_impl.cc',
        'db/db_impl.h',
        'db/db_iter.cc',
        'db/db_iter.h',
        'db/filename.cc',
        'db/filename.h',
        'db/log_format.h',
        'db/log_reader.cc',
        'db/log_reader.h',
        'db/log_writer.cc',
        'db/log_writer.h',
        'db/memtable.cc',
        'db/memtable.h',
        'db/repair.cc',
        'db/skiplist.h',
        'db/snapshot.h',
        'db/table_cache.cc',
        'db/table_cache.h',
        'db/version_edit.cc',
        'db/version_edit.h',
        'db/version_set.cc',
        'db/version_set.h',
        'db/write_batch.cc',
        'db/write_batch_internal.h',
        'include/leveldb/cache.h',
        'include/leveldb/c.h',
        'include/leveldb/comparator.h',
        'include/leveldb/db.h',
        'include/leveldb/env.h',
        'include/leveldb/filter_policy.h',
        'include/leveldb/iterator.h',
        'include/leveldb/options.h',
        'include/leveldb/slice.h',
        'include/leveldb/status.h',
        'include/leveldb/table_builder.h',
        'include/leveldb/table.h',
        'include/leveldb/write_batch.h',
        'port/atomic_pointer.h',
        'port/port.h',
        'port/port_posix.cc',
        'port/port_posix.h',
        'port/thread_annotations.h',
        'table/block_builder.cc',
        'table/block_builder.h',
        'table/block.cc',
        'table/block.h',
        'table/filter_block.cc',
        'table/filter_block.h',
        'table/format.cc',
        'table/format.h',
        'table/iterator.cc',
        'table/iterator_wrapper.h',
        'table/merger.cc',
        'table/merger.h',
        'table/table_builder.cc',
        'table/table.cc',
        'table/two_level_iterator.cc',
        'table/two_level_iterator.h',
        'util/arena.cc',
        'util/arena.h',
        'util/bloom.cc',
        'util/cache.cc',
        'util/coding.cc',
        'util/coding.h',
        'util/comparator.cc',
        'util/crc32c.cc',
        'util/crc32c.h',
        'util/env.cc',
        'util/env_posix.cc',
        'util/filter_policy.cc',
        'util/hash.cc',
        'util/hash.h',
        'util/histogram.cc',
        'util/histogram.h',
        'util/logging.cc',
        'util/logging.h',
        'util/mutexlock.h',
        'util/options.cc',
        'util/posix_logger.h',
        'util/random.h',
        'util/status.cc'
      ],
      'sources/': [
        ['exclude', '_(android|example|portable)\\.cc$'],
      ],
    },
    {
      'target_name': 'leveldb_testutil',
      'type': '<(library)',
      'dependencies': [
        'leveldb',
      ],
      'export_dependent_settings': [
        'leveldb',
      ],
      'sources': [
        'util/histogram.cc',
        'util/histogram.h',
        'util/testharness.cc',
        'util/testharness.h',
        'util/testutil.cc',
        'util/testutil.h',
      ],
    },
    {
      'target_name': 'leveldb_arena_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'util/arena_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_cache_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'util/cache_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_coding_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'util/coding_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_corruption_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/corruption_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_crc32c_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'util/crc32c_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_db_bench',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/db_bench.cc',
      ],
    },
    {
      'target_name': 'leveldb_db_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/db_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_dbformat_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/dbformat_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_env_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'util/env_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_filename_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/filename_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_log_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/log_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_skiplist_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/skiplist_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_table_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'table/table_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_version_edit_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/version_edit_test.cc',
      ],
    },
    {
      'target_name': 'leveldb_write_batch_test',
      'type': 'executable',
      'dependencies': [
        'leveldb_testutil',
      ],
      'sources': [
        'db/write_batch_test.cc',
      ],
    },
  ],
}

# Local Variables:
# tab-width:2
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=2 shiftwidth=2:
