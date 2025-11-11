import express from "express";
import mongoose from "mongoose";
import cors from "cors";

const app = express();
app.use(cors());
app.use(express.json({ limit: "50kb" }));
app.use(express.urlencoded({ extended: true }));
app.use(express.text()); // <--- add this one
 // ✅ Required to read JSON POST body

// ✅ MongoDB Atlas connection
const MONGO_URI = process.env.MONGO_URI || "mongodb+srv://geshybarbia_db_user:geshy123456@thesis.yrcxczx.mongodb.net/ambulance_gps";

mongoose
  .connect(MONGO_URI)
  .then(() => console.log("✅ MongoDB Connected"))
  .catch((err) => console.log("❌ MongoDB Error:", err));

// ✅ GPS Schema & Model
const gpsSchema = new mongoose.Schema(
  {
    lat: Number,
    lon: Number,
    speed: Number,
    alt: Number,
    vsat: Number,
    usat: Number,
    acc: Number
  },
  { timestamps: true }
);

const GPS = mongoose.model("gps_logs", gpsSchema);

// ✅ ESP32 sends POST (not GET)
app.post("/post_gps", async (req, res) => {
  try {
    console.log("📥 Incoming GPS Data:", req.body);

    await GPS.create(req.body);

    return res.status(200).json({ status: "saved" });
  } catch (err) {
    console.log("❌ Insert Error:", err);
    return res.status(500).json({ error: "DB Error", details: err.message });
  }
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`🚀 Server running on port ${PORT}`));
