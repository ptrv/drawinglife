#!/usr/bin/env ruby

# Script for changing the date of Gps entres in a database.
#
# Usage e.g $ ruby ModifyGpsDate.rb test.db '2011-01-28'"
#
# Peter Vasil
# Date: 2011-01-28

require 'rubygems'

require 'sqlite3'

# check if number of arguments is ok
unless ARGV.length == 2
  puts "Not the right number of arguments."
  puts "Usage: ruby ModifyGpsDate.rb test.db '2011-01-28'\n"
  exit
end

database_path = ARGV[0]
date_to_use = ARGV[1]


db = SQLite3::Database.new(database_path)

# Get all user ids and names
users = db.execute( "SELECT * FROM user" )

puts "Number of users: #{users.length}"

# date_to_use = "2011-03-19"

# Create all arrays
datetimes = Array.new(Array.new)
datetimes_new = Array.new(Array.new)
user_ids = Array.new

# Get all timestamps for all users
users.each do |user|

  datetime = Array.new
  # puts "User: #{user[0]}, name: #{user[1]}"
  db.execute("SELECT * FROM gpsdata WHERE user = ? ORDER BY datetime(time) ASC", user[0]).each do |row|
    datetime.push(row[5])
  end

  datetime_new = Array.new(datetime.length)

  datetime.length.times do |i|
    index_t = datetime[i].index('T');
    current_date = datetime[i]
    datetime_new[i] = ""
    datetime_new[i] << date_to_use
    datetime_new[i] << current_date.slice(index_t..current_date.length)
  end

  datetimes.push(datetime)
  datetimes_new.push(datetime_new)
  user_ids.push(user[0].to_s)
end

puts "Modifying dates..."

num_modified = 0

# update database entries 
db.transaction
datetimes.length.times do |i|
  datetimes[i].length.times do |j|
    db.execute("UPDATE gpsdata SET time = :newdate WHERE time = :olddate AND user = :user",
               "newdate" => datetimes_new[i][j],
               "olddate" => datetimes[i][j],
               "user" => user_ids[i])

    num_modified += 1
    
  end
end
db.commit
db.close

puts "Modified #{num_modified} entries!"
