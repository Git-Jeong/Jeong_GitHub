export type ButtonType = 
  | 'number'
  | 'operator'
  | 'function'
  | 'equals'
  | 'clear';
export interface ButtonProps {
  label: string;
  type: ButtonType;
  onClick: () => void;
}


