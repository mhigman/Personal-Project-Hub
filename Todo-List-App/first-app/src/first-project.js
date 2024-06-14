import React from "react";
import { TextField, Box, InputAdornment, Button } from '@mui/material';
import { NumericFormat } from 'react-number-format';


const FirstProject = () => {
    const [status, updateStatus] = React.useState("");
    let formData = JSON.parse(localStorage.getItem("formData")) ? JSON.parse(localStorage.getItem("formData")) : {
        timeValue: '0',
        timeInterval: '0',
        numOfBlocks: '0'
    };
    //console.log(formData)
    //document.getElementById("progress-bar-time").value = formData['timeValue'];
    //document.getElementById("progress-bar-interval").value = formData['timeInterval'];
    //document.getElementById("progress-bar-length").value = formData['numOfBlocks'];
    const handleChange = (e) => {
        updateStatus("");
        formData[e.target.name] = e.target.value;
        localStorage.setItem("formData", JSON.stringify(formData));
    };
    function OnSubmit() {
        formData['timeValue'] = document.getElementById("progress-bar-time").value;
        formData['timeInterval'] = document.getElementById("progress-bar-interval").value;
        formData['numOfBlocks'] = document.getElementById("progress-bar-length").value;
        fetch("/create-progress-bar", {
            method: 'POST',
            body: JSON.stringify(formData),
            headers: {
                "Content-Type": "application/json"
            }
        })
            .then((res) => res.json())
            .then((data) => updateStatus(data.status));
    }
    return (
        //<div className="App">
        <div className="App-body">

            <h1 className='page-title'>Progress Bar</h1>
            <Box
                component="form"
                sx={{
                    '& > :not(style)': { m: 1, width: '25ch' },
                }}
                noValidate
                autoComplete="off"
                className="progress-form">
                <ul className='hidden-list'>
                    <TextField id="progress-bar-time"
                        helperText="Progress Bar Time"
                        sx={{ m: 1, width: '25ch' }}
                        InputProps={{
                            endAdornment: <InputAdornment position='end'>secs</InputAdornment>,
                            inputComponent: NumericFormat,
                        }}
                        variant='standard'
                        name='timeValue'
                        onChange={handleChange}
                        defaultValue={formData['timeValue']}
                    />
                    <TextField id="progress-bar-interval"
                        helperText="Progress Interval"
                        sx={{ m: 1, width: '25ch' }}
                        InputProps={{
                            endAdornment: <InputAdornment position='end'>secs</InputAdornment>,
                            inputComponent: NumericFormat,
                        }}
                        variant='standard'
                        name='timeInterval'
                        onChange={handleChange}
                        defaultValue={formData['timeInterval']}
                    />
                    <TextField id="progress-bar-length"
                        helperText="Progress Bar Length"
                        sx={{ m: 1, width: '25ch' }}
                        InputProps={{
                            //startAdornment: <InputAdornment position='end'>secs</InputAdornment>,
                            inputComponent: NumericFormat,
                        }}
                        variant='standard'
                        name='numOfBlocks'
                        onChange={handleChange}
                        defaultValue={formData['numOfBlocks']}
                    />
                </ul>
                <Button onClick={OnSubmit}>Submit</Button>
                <p>Status: {status}</p>
            </Box>
        </div>
    );
}
export default FirstProject;