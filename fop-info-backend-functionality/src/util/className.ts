const className = <T>(from: T | any, ...classes: (keyof T | any)[]) => {
    let validNames = [];

    for (let name of classes) {
        if (typeof(name) == 'string')
            validNames.push(from[name])
    }
    
    return validNames.filter(x => x).join(' ')
}

export default className;
