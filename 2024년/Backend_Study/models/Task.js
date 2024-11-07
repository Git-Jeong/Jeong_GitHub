import mongoose from 'mongoose';

const TaxkSchema = new mongoose.Schema(
    {
        title:{
            type: String,
        },
        description:{
            type: String,
        },
        isConplete:{
            type: String,
            default: false,
        },
    },
    {
        timestamps:true,
    }
);

const Task = mongoose.model('Task', TaxkSchema);
            //몽고db에서 Task라는 이름의 컬렉션을 만드는 것
export default Task;