-- This is an example query file for DrawingLifeApp.
-- -------------------------------------------------
-- The following part of the query is already in the program.
-- There is no need to put it in the query.
-- -------------------------------------------------
-- "SELECT a.trackpoint_uid, y(a.geom) AS latitude, x(a.geom) AS longitude,
-- a.utctimestamp AS time, a.ele AS elevation,
-- a.trkseg_id AS segment, b.username AS name
-- FROM trackpoints AS a
-- JOIN users AS b ON (a.user_uid = b.user_uid)
-- -------------------------------------------------
-- Pleas do the query from here

WHERE b.username = 'John' ORDER BY datetime(time)
