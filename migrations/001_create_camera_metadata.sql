CREATE TABLE IF NOT EXISTS camera_metadata
(
    cam_id
    UUID
    PRIMARY
    KEY
    DEFAULT
    gen_random_uuid
(
),
    image_id UUID,
    camera_name TEXT NOT NULL,
    firmware_version TEXT NOT NULL,
    container_name TEXT,
    name_of_stored_picture TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    onboarded_at TIMESTAMP,
    initialized_at TIMESTAMP
    );
