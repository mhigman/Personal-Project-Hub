import React from "react";
import { TextField, Button } from "@mui/material";
import validator from "validator";
import currentUser from "./user";
//import ws from "./socket";
function RegisterPage(){

    const [isValidPassword, setValidity] = React.useState(false);
    const [registerStatus, setRegisterStatus] = React.useState("");


    async function onRegister(e){ 
        await fetch("http://localhost:8080/register-user", {
            method: "POST",
            body: JSON.stringify(  
                {
                    username: document.querySelector("#username-input").value,
                    password: document.querySelector("#password-input").value
                }
            ),
            headers: {
                "Content-Type" : "application/json"
            }
        })
        .then((res) => {
            console.log(res.body);
            //console.log(res.json());
            return res.json();
        })
        .then((userData) => {
            console.log(userData);
            currentUser.username = userData.username;
            currentUser.password = userData.password;
        });
        
        console.log(currentUser);
        if (currentUser.username !== ""){
            sessionStorage.setItem('currentUser', JSON.stringify(currentUser));
            setRegisterStatus("You have been registered!");  
        } else {
            setRegisterStatus("Username Taken");
        }

        setTimeout(3000, () => {
            setRegisterStatus("");
        });
    }
    const validate = (e) => {
        setValidity(
            validator.isStrongPassword(e.target.value, {
                minLength: 8, minLowercase: 1,
                minUppercase: 1, minNumbers: 1,
                minSymbols: 1
            })
        );
    }

    return (
        <div className="App-body">
            <h1 className="page-title">Register</h1>
            <div className="page-content">
                <TextField 
                            id="username-input" 
                            label="Username" 
                            style={{width: "100%",
                                    marginTop: "10px"}} 
                            inputProps={{maxLength: 18}}
                />

                <TextField 
                        error={isValidPassword}
                        id="password-input" 
                        label="Password"
                        type="password" 
                        style={{width: "100%",
                                marginTop: "10px"}} 
                        inputProps={{maxLength: 18}}
                        onKeyDown={validate}
                />
                <Button onClick={onRegister}>Submit</Button>

                <h3>{registerStatus}</h3>
            </div>
        </div>
    );
}

export default RegisterPage;