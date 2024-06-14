import React, { useEffect } from 'react';
import './App.css';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import Navbar from './navbar';
import Chatroom from './chatroom';
import LoginPage from './login';
import RegisterPage from './register';
import Footer from './footer';
import currentUser from './user';

//import TextField from '@mui/material/TextField';
//import Hidden from '@mui/material/Hidden';
function App() {
  
  useEffect(() => {
    console.log("Getting Previous User...");
    console.log(sessionStorage.getItem("currentUser"));
    const loggedInUser = JSON.parse(sessionStorage.getItem("currentUser"));
    console.log(loggedInUser);
    if (loggedInUser) {
      currentUser.username = loggedInUser.username;
      currentUser.password = loggedInUser.password;
      console.log(currentUser);
    }
  }, []);
    
    
    
  return (
    <Router>
      <Navbar/>
      <Routes>
        <Route exact path="/" element={<Chatroom />}/>
        <Route path="/login" element={<LoginPage/>}/>
        <Route path="/register" element={<RegisterPage/>}/>
      </Routes>
      <Footer/>
    </Router>
  );
}
window.onbeforeunload = () => {
  sessionStorage.setItem("messageHistory", 
  document.querySelector("#chat").value);
};
export default App;
