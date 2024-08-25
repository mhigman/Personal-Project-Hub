import { ButtonHTMLAttributes, DetailedHTMLProps, FC } from "react";
import style from "./style.module.scss";
import className from "@/util/className";

export type ButtonProps = {
    variant?: 'cta' | 'ghost';
    size?: 'medium' | 'small';
} & DetailedHTMLProps<ButtonHTMLAttributes<HTMLButtonElement>, HTMLButtonElement>;

const Button: FC<ButtonProps> = ({ variant, size, ...other }) => {
    return (
        <button {...other} className={
            className(style, 'button', variant ?? 'ghost', size ?? 'medium')
        } />
    );
};

export default Button;
