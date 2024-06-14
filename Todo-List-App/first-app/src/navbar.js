import React from 'react';
import { NavLink as Link } from 'react-router-dom';
import styled from '@emotion/styled';

const NavLink = styled(Link)`
    color: #000000;
    text-decoration: none;
    &.active{
        color: #00c8ff;
    }
`;

const Navbar = () => {
  return (
    <div className='App-header'>
      <h6 className='corner-logo'>Random Stuff</h6>
      <ul className='nav-bar'>
        <li><NavLink to='/'>Home</NavLink></li>
        <li><NavLink to='/first-project'>First Project</NavLink></li>
        <li><NavLink to='/about'>About</NavLink></li>
        <li><NavLink to='/future-project'>Future Projects</NavLink></li>
        <li><NavLink to='/todo'>Saved Todo List</NavLink></li>
      </ul>
    </div>
  );
}

export default Navbar;