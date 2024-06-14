import React from "react";
import { TextField, Button } from "@mui/material";
import currentUser from "./user";
import validator from "validator";
//import { json } from "react-router-dom";
function LoginPage(){

    const [isValidPassword, setValidity] = React.useState(false);
    const [loginStatus, setLoginStatus] = React.useState("");

    async function onSubmit(e){
        await fetch("http://localhost:8080/login-user", {
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
            //console.log(res.text());
            return res.json();
        })
        .then((userData) => {
            console.log(userData);
            currentUser.username = userData.username;
            currentUser.password = userData.password;
        });

        if(currentUser.username !== ""){
            sessionStorage.setItem('currentUser', JSON.stringify(currentUser));
            console.log(currentUser);
            setLoginStatus("Logged In");
        } else {
            setLoginStatus("Invalid Username and Password");
        }
        

        await setTimeout(3000, () => {
            setLoginStatus("");
        })
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
            <h1 className="page-title">Login</h1>
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
                <Button onClick={onSubmit}>Submit</Button>

                <h3>{loginStatus}</h3>
            </div>
        </div>
    );

}

export default LoginPage;