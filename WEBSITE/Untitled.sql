-- Drop table water_quality if needed
DROP TABLE IF EXISTS water_quality CASCADE;

CREATE TABLE water_quality (
    id SERIAL PRIMARY KEY,
    ph NUMERIC(4,2) NOT NULL,
    tds INTEGER NOT NULL,
    turbidity NUMERIC(4,2) NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Enable Row Level Security
ALTER TABLE water_quality ENABLE ROW LEVEL SECURITY;

-- Allow SELECT for all users (public read-only)
CREATE POLICY "Allow read access to all"
    ON water_quality
    FOR SELECT
    USING (true);
