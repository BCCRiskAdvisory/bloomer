Gem::Specification.new do |s|
  s.name = 'Bloomer'
  s.version = '0.0.0'
  s.date = '2014-11-10'
  s.summary = 'Quick full text search cache'
  s.authors = 'Owen Mooney'
  s.files = Dir['lib/**/*']
  s.extensions = ['ext/bloomer/extconf.rb']
end