import React from "react";
import {NavLink as Link} from 'react-router-dom';
import styled from "@emotion/styled";

const NavLink = styled(Link)`
    color: #000000;
    text-decoration: none
    &.active{
        color: #00c8ff;
    }
`;

const Navbar = () => {
    return (
        <div className="App-header">
            <h6 className="corner-logo">Chatroom I think</h6>
            <ul className="nav-bar">
                <li><NavLink to='/'>Prototype</NavLink></li>
                <li><NavLink to='/login'>Login</NavLink></li>
                <li><NavLink to='/register'>Register</NavLink></li>
            </ul>
        </div>
    );
}


export default Navbar;