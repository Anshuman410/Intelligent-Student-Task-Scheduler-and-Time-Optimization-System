const PDFDocument = require("pdfkit");
const fs = require("fs");
const { Task } = require("./tasks"); 
exports.generateReport = async (req, res) => {
  try {
    const userId = req.params.userId;
    const lastWeek = new Date();
    lastWeek.setDate(lastWeek.getDate() - 7);

    const tasks = await Task.find({
      userId,
      createdAt: { $gte: lastWeek }
    });

    if (!fs.existsSync("./reports")) fs.mkdirSync("./reports");
    const file = `./reports/report_${userId}.pdf`;

    const doc = new PDFDocument();
    const writeStream = fs.createWriteStream(file);

    doc.pipe(writeStream);
    doc.fontSize(22).text("Weekly Task Report", { align: "center" });
    doc.moveDown();

    let done = 0, pending = 0;
    tasks.forEach(t => { t.completed ? done++ : pending++; });

    doc.fontSize(14).text(`Summary for Last 7 Days`);
    doc.text(`Total Tasks: ${tasks.length}`);
    doc.text(`Completed: ${done}`);
    doc.text(`Pending: ${pending}`);
    doc.moveDown().text("Details:", { underline: true });

    tasks.forEach((t, i) => {
      doc.moveDown(0.5);
      doc.fontSize(12).text(`${i + 1}. ${t.title} [${t.completed ? "DONE" : "PENDING"}]`);
      doc.fontSize(10).text(`Category: ${t.category} | Priority: ${t.priority}`);
    });

    doc.end();

    writeStream.on("finish", () => {
      res.download(file);
    });
  } catch (error) {
    res.status(500).send("Report generation failed");
  }
};