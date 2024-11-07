import React from 'react';
import { DisplayProps } from '../types/DisplayTypes';

const Display: React.FC<DisplayProps> = ({ value }) => {
  return (
    <div className="display">
      {value}
    </div>
  );
};

export default Display;