SELECT a.trkpt_uid, y(a.geom) AS latitude, x(a.geom) AS longitude,
a.utctimestamp AS time, a.ele AS elevation,
a.trkseg_id AS segment, b.username AS name,
c.city AS city
FROM trackpoints AS a
JOIN users AS b ON (a.user_uid = b.user_uid)
JOIN citydefs AS c ON (a.citydef_uid = c.citydef_uid)
--WHERE b.username = 'John'
ORDER BY datetime(time)
