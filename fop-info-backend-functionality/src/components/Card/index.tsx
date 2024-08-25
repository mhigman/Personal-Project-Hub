import { DetailedHTMLProps, FC, HTMLAttributes, ReactNode } from "react";
import style from "./style.module.scss";
import className from "@/util/className";

export type CardProps = {
    children?: ReactNode;
    solid?: boolean;
} & DetailedHTMLProps<HTMLAttributes<HTMLDivElement>, HTMLDivElement>;

const Card: FC<CardProps> = ({ solid, ...other }) => {
    return (
        <div {...other} className={className(style, 'card', solid && 'solid')} />
    );
};

export default Card;
