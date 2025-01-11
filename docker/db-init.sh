#!/bin/bash

echo "Initializing the database..."

PGPASSWORD=$POSTGRES_PASSWORD psql -v ON_ERROR_STOP=1 --username "$POSTGRES_USER" --dbname "$POSTGRES_DB" <<-EOSQL
    CREATE TABLE IF NOT EXISTS sample_table (
        id SERIAL PRIMARY KEY,
        data TEXT NOT NULL
    );

    INSERT INTO sample_table (data)
    SELECT * FROM (VALUES
        ('Hello, World!'),
        ('Sample Data 1'),
        ('Sample Data 2')
    ) AS new_data(data)
    WHERE NOT EXISTS (SELECT 1 FROM sample_table);
EOSQL

echo "Database initialization complete."
