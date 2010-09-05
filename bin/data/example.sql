-- This is an example query file for DrawingLifeApp.
-- -------------------------------------------------
-- The following part of the query is already in the program. 
-- There is no need to put it in the query.
-- -------------------------------------------------
-- SELECT a.gpsdataid AS gpsdataid, a.latitude AS latitude,
-- a.longitude AS longitude, a.time AS time,
-- a.elevation AS elevation, a.segment AS segment,
-- b.name AS name,
-- c.city AS location
-- FROM gpsdata AS a
-- JOIN user AS b ON (a.user = b.userid)
-- JOIN location AS c ON (a.location = c.locationid)
-- -------------------------------------------------
-- Pleas do the query from here

WHERE name = 'Dan' AND c.city = 'Berlin' ORDER BY datetime(time)
--WHERE name ='Dan' AND a.latitude < 52.55 AND a.latitude > 52.45 AND a.longitude < 13.40 AND a.longitude > 13.3 ORDER BY datetime(time)
