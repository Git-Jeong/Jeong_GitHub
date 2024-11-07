import React, { useState } from 'react';
import Display from './Display';
import Button from './Button';
import { ButtonType } from '../types/ButtonTypes';

const scientificFunctions = ['sin', 'cos', 'tan', 'log', 'exp'];

const Calculator: React.FC = () => {
  const [displayValue, setDisplayValue] = useState<string>('0');

  const handleButtonClick = (label: string) => {
    if (scientificFunctions.includes(label)) {
      const result = calculateScientificFunction(label, displayValue);
      setDisplayValue(result);
    } else if (label === 'C') {
      setDisplayValue('0');
    } else if (label === '=') {
      try {
        setDisplayValue(eval(displayValue)); // eval 사용은 보안상 위험할 수 있음
      } catch {
        setDisplayValue('Error');
      }
    } else {
      setDisplayValue((prev) => (prev === '0' ? label : prev + label));
    }
  };

  const calculateScientificFunction = (func: string, value: string): string => {
    const numValue = parseFloat(value);
    switch (func) {
      case 'sin':
        return Math.sin(numValue).toString();
      case 'cos':
        return Math.cos(numValue).toString();
      case 'tan':
        return Math.tan(numValue).toString();
      case 'log':
        return Math.log10(numValue).toString();
      case 'exp':
        return Math.exp(numValue).toString();
      default:
        return value;
    }
  };

  const buttons: { label: string; type: ButtonType }[] = [
    { label: '7', type: 'number' },
    { label: '8', type: 'number' },
    { label: '9', type: 'number' },
    { label: '/', type: 'operator' },
    { label: '4', type: 'number' },
    { label: '5', type: 'number' },
    { label: '6', type: 'number' },
    { label: '*', type: 'operator' },
    { label: '1', type: 'number' },
    { label: '2', type: 'number' },
    { label: '3', type: 'number' },
    { label: '-', type: 'operator' },
    { label: '0', type: 'number' },
    { label: '.', type: 'number' },
    { label: '=', type: 'equals' },
    { label: '+', type: 'operator' },
    { label: 'C', type: 'clear' }
  ];

  return (
    <div className="calculator">
      <Display value={displayValue} />
      <div className="buttons">
        {buttons.map((btn) => (
          <Button key={btn.label} label={btn.label} type={btn.type} onClick={() => handleButtonClick(btn.label)} />
        ))}
      </div>
      {scientificFunctions.map(func => (
        <Button key={func} label={func} type="function" onClick={() => handleButtonClick(func)} />
      ))}
    </div>
  );
};

export default Calculator;