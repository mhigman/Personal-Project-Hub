const formDataToJSON = (data: FormData) => {
    let json: any = {}

    for (let entry of data.entries())
         json[entry[0]] = entry[1]

    return JSON.stringify(json);
}

export default formDataToJSON
