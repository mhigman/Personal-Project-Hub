import { DetailedHTMLProps, FC, OlHTMLAttributes, ReactNode } from "react";
import Button from "../Button";
import Link, { LinkProps } from "next/link";
import style from "./style.module.scss"
import className from "@/util/className";
import Container from "../Container";

const AppbarItem: FC<LinkProps & {
    children?: ReactNode;
    active?: boolean;
}> = ({ active, ...props }) => {
    return (
        <li>
            <Link className={className(style, active && 'active')} {...props} />
        </li>
    )
}

const Spacer: FC = () => <div className={style.spacer} />

const Appbar: FC<
    DetailedHTMLProps<OlHTMLAttributes<HTMLUListElement>, HTMLUListElement>
> & {
    Item: typeof AppbarItem;
    Spacer: typeof Spacer;
} = (ulProps) => {
    return (
        <nav className={style.appbar} style={{ marginBottom: 24}}>
            <Container padded>
                <div className={style.logo}>{/* logo */}</div>
                <ul {...ulProps} />

                <Spacer />

                <Button variant='cta'>Donate</Button>
            </Container>
        </nav>
    )
}

Appbar.Item = AppbarItem
Appbar.Spacer = Spacer;

export default Appbar;
