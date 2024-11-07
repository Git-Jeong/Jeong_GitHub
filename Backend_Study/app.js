import express from 'express';
import tasks from "./data/mock.js"; // 가상의 task 데이터가 있는 파일을 임포트합니다.

const app = express(); // Express 앱을 생성합니다.
app.use(express.json()); // JSON 형식의 요청 본문을 자동으로 파싱해주는 미들웨어를 사용합니다.

// 간단한 테스트 라우트. '/hello' 경로에 GET 요청이 오면 'Hello Express' 메시지를 반환합니다.
app.get('/hello', (req, res) => {
    res.send('Hello Express'); // 클라이언트에게 'Hello Express'라는 응답을 전송합니다.
});

// '/tasks' 경로에 GET 요청이 오면 tasks 데이터를 쿼리 파라미터에 따라 정렬하고 필터링하여 반환합니다.
app.get('/tasks', (req, res) => {
    // 쿼리 파라미터로 정렬 기준 (sort)와 출력할 태스크의 수 (count)를 받습니다.
    const sort = req.query.sort;
    const count = Number(req.query.count);

    // 정렬 기준에 따라 비교 함수를 설정합니다.
    // sort 값이 'oldest'이면 오래된 순서, 그렇지 않으면 새로운 순서로 정렬합니다.
    const compareFn = 
        sort === 'oldest'
            ? (a, b) => a.createAt - b.createAt // 오래된 순서로 정렬
            : (a, b) => b.createAt - a.createAt; // 새로운 순서로 정렬

    // tasks 배열을 설정한 비교 함수에 따라 정렬합니다.
    let newTasks = tasks.sort(compareFn);

    // count가 있을 경우 count 만큼의 태스크만 반환하도록 슬라이스합니다.
    if(count){
        newTasks = newTasks.slice(0, count);
    }

    res.send(newTasks); // 클라이언트에게 정렬/필터링된 태스크 목록을 전송합니다.
});

// 특정 태스크를 ID를 통해 조회하는 라우트
app.get('/tasks/:id', (req, res) => {
    // URL의 파라미터로부터 ID를 가져옵니다.
    const id = Number(req.params.id);
    // tasks 배열에서 해당 ID의 태스크를 찾습니다.
    const task = tasks.find((task) => task.id === id);
    
    if(task){
        res.send(task); // ID가 일치하는 태스크를 찾으면 전송합니다.
    } else {
        // ID가 일치하는 태스크를 찾지 못하면 404 상태와 함께 에러 메시지를 전송합니다.
        res.status(404).send({ message: 'Cannot find given ID' });
    } 
});

// 새로운 태스크를 추가하는 라우트
app.post('/tasks', (req, res) => {
    const newTask = req.body; // 요청 본문에서 새로운 태스크 데이터를 가져옵니다.

    // 현재 tasks 배열의 모든 ID를 추출하고, 가장 큰 ID에 1을 더하여 새 태스크의 ID로 설정합니다.
    const ids = tasks.map((task) => task.id);
    newTask.id = Math.max(...ids) + 1;

    // 새로운 태스크의 기본 속성 설정: 완료 상태는 false, 생성 및 수정 시간은 현재 시간으로 설정합니다.
    newTask.isComplete = false;
    newTask.createAt = new Date();
    newTask.updateAt = new Date(); 

    tasks.push(newTask); // 새 태스크를 tasks 배열에 추가합니다.
    res.status(201).send(newTask); // 201 상태와 함께 새로 추가된 태스크를 응답합니다.
});


app.patch('/tasks/:id', (req, res) => {
    const id = Number(req.params.id);
    const task = tasks.find((task) => task.id === id);
    
    if(task){
        Object.keys(req.body).forEach((key)=>{
            task[key] = req.body[key];
        });
        task.updatedAt = new Date();
        res.send(task);
    } else {
        res.status(404).send({ message: 'Cannot find given ID' });
    } 
});

app.delete('/tasks/:id', (req, res) =>{
    const id = Number(req.params.id);
    const idx = tasks.findIndex((task) => task.id === id);

    if(idx >= 0){
        tasks.splice(idx, 1);
        res.sendStatus(204);
    }
    else{
        res.status(404).send({message: 'Cannot find given ID'});
    }
});


// 서버를 3000번 포트에서 시작합니다. 서버가 시작되면 콘솔에 메시지를 출력합니다.
app.listen(3000, () => console.log('Server Started'));
