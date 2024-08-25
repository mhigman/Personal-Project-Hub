const fileToBuffer = (file: File) => {
    return new Promise(resolve => {
        const reader = new FileReader()

        const readFile = function() {
            const buffer = reader.result
            resolve(buffer)
        }
    
        reader.addEventListener('load', readFile)
        reader.readAsArrayBuffer(file)
    })
}

export default fileToBuffer;
