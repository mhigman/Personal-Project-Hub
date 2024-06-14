import React from "react";
import { TextField, Button } from "@mui/material";
import useWebSocket from "react-use-websocket";
import {setSocketListeners, validMessage} from "./socket";  
import currentUser from "./user";  
let loggedIn = true;
function Chatroom() {
    let messageHistory = "";
    
    if(sessionStorage.getItem("messageHistory") !== null || sessionStorage.getItem("messageHistory") !== ""){
        messageHistory = sessionStorage.getItem("messageHistory");
    }
    console.log(messageHistory);
    
    
    /*
    const handleUserField = (e) => {
        if(e.key === "Enter"){
            console.log("websocket established");
            loggedIn=true;
            currentUser = document.querySelector("#user-thing").value;
            ws.send(document.querySelector("#user-thing").value);
            document.querySelector("#input").value = "";
            e.target.disabled = true;
        }
        
        
    } */
    const ws = setSocketListeners();
    const handleInputs = (e) => {
        console.log(e.key);
        if (e.key === "Enter"){
            
            if(currentUser.username === ""){
                e.target.value = "Please Login first"
                return;
            }
            
            //console.log(ws);
            if(loggedIn){
                if(validMessage){
                    loggedIn = false;
                } else{
                    console.log(validMessage, loggedIn);
                    ws.send(JSON.stringify(currentUser));
                }
            }
            console.log("send nessage");
            ws.send(e.target.value, currentUser);
            e.target.value = "";
            
        }
    }
    //<Button id="join" onClick={onSubmit}>Submit</Button>
    /*<TextField 
                    id="user-thing" 
                    label="Username" 
                    style={{width: "100%",
                            marginTop: "10px"}} 
                    onKeyDown={handleUserField}
                    inputProps={{maxLength: 18}}
                    disabled = {loggedIn}
                    defaultValue={currentUser}
                />*/
    return (
        <div className="App-body">
            <h1 className="page-title">Chatroom Thing</h1>
            <div className="page-content">
                
                
                <div className="chat-area">
                    <textarea id="chat" disabled="true">{messageHistory}</textarea>

                    <TextField 
                        id="input" 
                        onKeyDown={handleInputs} 
                        style={{width: "100%",
                                marginBottom: "10px"}}
                        inputProps={{maxLength: 20}}
                        label="Chat" />
                </div>
            </div>
        </div>
    );
}

export default Chatroom;
