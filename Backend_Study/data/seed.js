import mongoose from 'mongoose';
import data from './mock.js';
import Task from '../models/Task.js';
import { DATABASE_URL } from '../env.js';
 
mongoose.connect(DATABASE_URL).then(() => console.log('Connected to DB'));
await Task.deleteMany({});
await Task.insertMany(data);

mongoose.connection.close();