Gem::Specification.new do |s|
  s.name = 'bloom_cache'
  s.version = '0.0.0'
  s.date = '2014-11-10'
  s.summary = 'Quick full text search cache'
  s.description = 'This gem provides a way of using bloom filters 
    to quickly discard database rows that do not match a string'
  s.authors = 'Owen Mooney'
  s.email = 'owen.m@bccriskadvisory.com'
  s.homepage = 'https://github.com/BCCRiskAdvisory/bloomer'
  s.files = Dir['lib/**/*']
  s.extensions = ['ext/bloom_cache/extconf.rb']
  s.add_development_dependency 'rspec', '~> 3'
end