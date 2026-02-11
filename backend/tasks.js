const mongoose = require("mongoose");

// Schema
const TaskSchema = new mongoose.Schema({
  userId: String,

  title: String,
  description: String,
  category: String,

  deadline: Date,
  priority: Number,
  dependencies: String,

  completed: {
    type: Boolean,
    default: false
  },

  createdAt: {
    type: Date,
    default: Date.now
  }
});

const Task = mongoose.model("Task", TaskSchema);


exports.addTask = async (req,res)=>{
 try{
  await Task.create(req.body);
  res.send("Added");
 }catch(e){
  res.status(500).send("Error");
 }
};


exports.getTasks = async (req,res)=>{
 const data = await Task.find({
  userId:req.params.id
 }).sort({deadline:1});

 res.json(data);
};


exports.completeTask = async (req,res)=>{
 await Task.findByIdAndUpdate(
  req.params.taskId,
  {completed:true}
 );
 res.send("Done");
};

exports.deleteTask = async (req,res)=>{
 await Task.findByIdAndDelete(req.params.taskId);
 res.send("Deleted");
};

exports.updateTask = async (req,res)=>{
 await Task.findByIdAndUpdate(
  req.params.taskId,
  req.body
 );
 res.send("Updated");
};

exports.autoSchedule = async (req,res)=>{

 const tasks = await Task.find({
  userId:req.params.id,
  completed:false
 });

 const today = new Date();

 const scheduled = tasks.map(t=>{

  const diff =
   (new Date(t.deadline)-today)/(1000*60*60*24);

  const urgency = diff<=0?10:10/diff;

  const score = (t.priority*2)+urgency;

  return {...t._doc,score};
 });

 scheduled.sort((a,b)=>b.score-a.score);

 res.json(scheduled);
};

module.exports.Task = Task;
