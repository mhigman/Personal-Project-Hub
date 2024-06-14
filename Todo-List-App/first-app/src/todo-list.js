import React from "react";
import { TextField } from "@mui/material";


function TodoList() {
    const [todo, setTodo] = React.useState([]);

    const handleAppend = (e) => {
        if(e.key === "Enter"){
            fetch("/write-to-todo", {
            method: "POST", 
            body: JSON.stringify( {text: e.target.value.replace('\n', '')}),
            headers: {
                "Content-Type": "application/json"
            }});
            setTodo([...todo, "\u2022"+e.target.value.replace('\n', '')]);            
            e.target.value = "";
        }
    }

    const handleRemove = (e) => {
        fetch("/remove-todo", {
            method: "POST",
            body: JSON.stringify( {text: e.target.id} ),
            headers: {
                "Content-Type": "application/json"
            }
        });
        setTodo(todo.filter((item) => item !== e.target.id));
    };
    React.useEffect(() =>{
        fetch("/get-todo-list")
            .then((res) => res.json())
            .then((data) => setTodo(data.text.split('⌫')))
    }, []);
    const todoHtml = todo.map((task) =>
        {return task !== "" ? <>
        {task}     <button id={task} onClick={handleRemove}>⌫</button> {'\n'}
        </> : <></>
    });
    return (
        <div className="App-body">
            <h1 className="page-title">Todo List</h1>
            <div className="page-content-no-indent">
                <div className="todo-text">{todoHtml}</div>
                <TextField id="todo-entry"
                    label="Enter Your message"
                    variant="standard"
                    fullWidth
                    multiline
                    onKeyUp={handleAppend}/>
            </div>
        </div>


    );
}

export default TodoList;