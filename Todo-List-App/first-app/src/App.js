//import logo from './logo.svg';
import React from 'react';
import './App.css';
import {BrowserRouter as Router, Routes, Route} from 'react-router-dom';
import FirstProject from './first-project';
import Home from './home';
import About from './about';
import FutureProject from './future-project';
import Navbar from './navbar';
import Footer from './footer';
import TodoList from './todo-list';


function App() {
  return(
    <Router>
      <Navbar />
      <Routes>
        <Route exact path="/" element={<Home/>}/>
        <Route path="/first-project" element={<FirstProject/>}/>
        <Route path="/about" element={<About/>}/>
        <Route path="/future-project" element={<FutureProject/>}/>
        <Route path="/todo" element={<TodoList/>}/>
      </Routes>
      <Footer/>
    </Router>
  ); 
}

export default App;
