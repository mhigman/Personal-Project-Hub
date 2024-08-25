import type { FC, ReactNode } from "react";

type FlexProps = {
    alignItems?: 'flex-start' | 'center' | 'flex-end' | 'stretch';
    justifyContent?: 'flex-start' | 'center' | 'flex-end' | 'stretch' | 'space-between' | 'space-around'
    flexDirection?: 'row' | 'row-reverse' | 'column' | 'column-reverse';
    gap?: number;
    children?: ReactNode;
};

const Flex: FC<FlexProps> = ({ children, ...css }) => {
    return <div style={{ display: 'flex', ...css }}>
        {children}
    </div>
};

export default Flex;
