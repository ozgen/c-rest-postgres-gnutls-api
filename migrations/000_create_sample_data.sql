CREATE TABLE IF NOT EXISTS sample_table
(
    id
    SERIAL
    PRIMARY
    KEY,
    data
    TEXT
    NOT
    NULL
);

INSERT INTO sample_table (data)
SELECT *
FROM (VALUES ('Hello, World!')) AS new_data(data)
WHERE NOT EXISTS (SELECT 1 FROM sample_table);