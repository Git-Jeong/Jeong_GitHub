import React from 'react';
import { ButtonProps } from '../types/ButtonTypes';
const Button: React.FC<ButtonProps> = ({ label, type, onClick }) => {
  return (
    <button className={`button ${type}`} onClick={onClick}>
      {label}
    </button>
  );
};

export default Button;


