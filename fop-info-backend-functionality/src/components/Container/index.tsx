import { DetailedHTMLProps, FC, HTMLAttributes, ReactNode } from "react";
import style from "./style.module.scss";
import className from "@/util/className";

export type ContainerProps = {
    children?: ReactNode;
    padded?: boolean;
} & DetailedHTMLProps<HTMLAttributes<HTMLDivElement>, HTMLDivElement>;

const Container: FC<ContainerProps> = ({ padded, ...other }) => {
    return (
        <section {...other} className={className(style, 'container', padded && 'padded')} />
    );
};

export default Container;
