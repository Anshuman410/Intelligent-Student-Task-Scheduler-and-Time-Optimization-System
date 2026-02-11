exports.getBadge = (points)=>{
  if(points<50) return "Beginner";
  if(points<100) return "Pro";
  return "Master";
};
