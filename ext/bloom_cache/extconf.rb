require 'mkmf'

have_library('crypto', 'OpenSSL_add_all_digests')

if enable_config('debug')
  puts "Enabling debugging symbols"
  CONFIG['debugflags'] << '-O0'
end

create_makefile 'bloom_cache/bloom_cache'