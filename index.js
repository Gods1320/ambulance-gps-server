import express from "express";
import mongoose from "mongoose";
import cors from "cors";

const app = express();
app.use(cors());
app.use(express.json());

// ✅ Replace with your Atlas URI
const MONGO_URI = process.env.MONGO_URI || "mongodb+srv://geshybarbia_db_user:geshy123456@thesis.yrcxczx.mongodb.net/ambulance_gps?retryWrites=true&w=majority&appName=Thesis";

mongoose.connect(MONGO_URI)
  .then(() => console.log("✅ MongoDB Connected"))
  .catch(err => console.log("❌ MongoDB Error:", err));

// ✅ Create GPS Model
const gpsSchema = new mongoose.Schema({
  lat: Number,
  lon: Number,
  speed: Number,
  alt: Number,
  vsat: Number,
  usat: Number,
  acc: Number,
  timestamp: { type: Date, default: Date.now }
});

const GPS = mongoose.model("gps_logs", gpsSchema);

// ✅ ESP32 Endpoint
app.get("/post_gps", async (req, res) => {
  try {
    await GPS.create(req.query);
    console.log("✅ GPS Saved:", req.query.lat, req.query.lon);
    res.send("OK");
  } catch (err) {
    console.log("❌ Insert Error:", err);
    res.status(500).send("DB Error");
  }
});

const port = process.env.PORT || 3000;
app.listen(port, () => console.log(`🚀 Server running on port ${port}`));
