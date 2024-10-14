import dash
from dash import dcc, html
from dash.dependencies import Input, Output
import plotly.graph_objs as go
import numpy as np

# Define the number of data points per x-coordinate
num_points_per_x_list = [3, 5, 4, 6, 7, 3, 3, 4, 5, 4, 2, 2, 1, 2] # Sample list of data points per x-coordinate

num_points_per_x_list.reverse()
# Define custom labels for x-axis
custom_labels = ['Reconnaissance', 
                 'Resource Development', 
                 'Inital Access', 
                 'Execution', 
                 'Persistance', 
                 'Privilege Escalation', 
                 'Defense Evaluation', 
                 'Credential Access', 
                 'Discovery', 
                 'Laterial Movement', 
                 'Collection',
                 'Command and Control',
                 'Exfiltration',
                 'Impact']

letters = 'abcdefghijklmnopqrstuvwxyz'

names = []

label_for_stages = []

# Generate x-coordinates based on the number of data points per x-coordinate
x_coordinates = []
for i, num_points in enumerate(num_points_per_x_list):
    x_coordinates.extend([len(num_points_per_x_list)-1-i] * num_points_per_x_list[-(i+1)])
    label_for_stages.extend([custom_labels[i]] * num_points_per_x_list[-(i+1)])
    for j in range(num_points_per_x_list[-(i+1)]):
        names.append(custom_labels[i] + f" ({letters[j]})")

print(label_for_stages)
# Generate random spherical data points on the x-coordinates

x = x_coordinates
num_layers = len(num_points_per_x_list)
print(x)
custom_labels.reverse()
# Initialize y-values list
z = []

# Generate random y-values for each layer
for i in range(num_layers):
    # Generate random probabilities for each data point in the layer
    probabilities = np.random.rand(num_points_per_x_list[-(i+1)])
    print(f"Stage {i}")
    print(sum(probabilities))
    # Normalize probabilities to ensure they sum up to 1
    probabilities /= np.sum(probabilities)
    print(probabilities)
    print(sum(probabilities))
    z.extend(probabilities)
print(z)
y = np.random.uniform(0, 1, len(x))

# Generate random sizes for spheres
sizes = np.random.uniform(0, 100, len(x))

print(x)

connections = []

for i in range(len(x)-1):
    for j in range(1, len(x)):
        if x[i]-1 == x[j]:
            connections.append((i,j))

attack_hover_text = [
    f"Name = {names[i]}<br>Stage = {label_for_stages[i]}<br>Severity = {y[i]}<br>Likelihood = {z[i]}<br>Protection = {1- sizes[i]/100}"
 for i in range(len(x))]

# Define scatter plot for data points
scatter = go.Scatter3d(
    name="Attacks",
    x=x,
    y=y,
    z=z,
    mode='markers',
    marker=dict(size=sizes, color='red'),
    hoverinfo='text',  # Show hover text
    text=attack_hover_text
)


def calculate_weight_at_const_rate(s, l_a, l_b, p):
    tolerance_of_severity = 0.5
    tolerance_of_likelihood = 0.5
   

    if l_a == 0 or l_b == 0:
        return 0
    l = (l_a * l_b) / l_a

    if s == 0:
        return 0
    
    w = (1-p)*(l + (1-l)*tolerance_of_likelihood)*(s + (1-s)*tolerance_of_severity)
    return w*5


# Define scatter plot for connections with different widths
lines = []
for connection in connections:
    i, j = connection
    w = calculate_weight_at_const_rate(y[j], z[i], z[j], 1-(sizes[j]/100))
    hover = f'Weight = {w/5}<br>Change in Severity = {y[j]-y[i]}<br>Change in Likelihood = {z[j]-z[i]}'
    line = go.Scatter3d(
        name=f"{names[i]}-{names[j]}",
        x=[x[i], x[j]],
        y=[y[i], y[j]],
        z=[z[i], z[j]],
        mode='lines',
        line=dict(color='blue', width=w),  # Random width for each line
        hoverinfo='text',  # Disable hover for connections
        hovertext=hover,
        meta=w/5  # Store weight as metadata
    )
    lines.append(line)

# Sort connections based on weight (descending order)
lines.sort(key=lambda line: line.meta, reverse=True)

# Define layout with gridlines and adjusted x-axis range
layout = go.Layout(
    scene=dict(
        xaxis=dict(title='Stages', gridcolor='white', zeroline=False, titlefont=dict(color='white'), tickfont=dict(color='white'), ticktext=custom_labels, tickvals=list(range(len(custom_labels)))),
        yaxis=dict(title='Severity', gridcolor='white', zeroline=False, titlefont=dict(color='white'), tickfont=dict(color='white')),
        zaxis=dict(title='Likelihood', gridcolor='white', zeroline=False, titlefont=dict(color='white'), tickfont=dict(color='white')),
        aspectmode='manual',  # Manual control of aspect ratio
        aspectratio=dict(x=2.5, y=1, z=1)  # Increase the relative length of the x-axis
    ),
    paper_bgcolor='grey'  # Set background color to black
)

# Create plotly figure
fig = go.Figure(data=[scatter, *lines], layout=layout)

fig.update_xaxes(
    gridcolor="black"
)
fig.update_yaxes(
    gridcolor="black"
)

# Initialize global variable to store clicked points
clicked_points = []

# Create Dash app
app = dash.Dash(__name__)

# Define app layout
app.layout = html.Div([
    dcc.Graph(
        id='graph',
        figure=fig,
        style={'width': '100vw', 'height': '100vh'}  # Set width and height to viewport width and height
    ),
])

# Define callback function for click event
@app.callback(Output('graph', 'figure'), [Input('graph', 'clickData')])
def draw_line(clickData):
    global clicked_points
    if clickData:
        if 'points' in clickData:
            points = clickData['points']
            if len(points) == 1:
                p = points[0]['pointNumber']
                clicked_points.append(p)
                print(clicked_points)

                if len(clicked_points) == 2:
                    p1, p2 = clicked_points
                    if(p1==p2):
                        clicked_points = []
                        return
                    if(p1>p2):
                        p2, p1 = p1, p2
                    x1, y1, z1 = fig['data'][0]['x'][p1], fig['data'][0]['y'][p1], fig['data'][0]['z'][p1]
                    x2, y2, z2 = fig['data'][0]['x'][p2], fig['data'][0]['y'][p2], fig['data'][0]['z'][p2]
                    w = calculate_weight_at_const_rate(y2, z1, z2, 1-(sizes[p2]/100))
                    line_trace_name1 = f"{names[p1]}-{names[p2]}"
                    line_trace_name2 = f"{names[p2]}-{names[p1]}"
                    line_index = None
                    for i, trace in enumerate(fig['data']):
                        if trace['name'] == line_trace_name1 or trace['name'] == line_trace_name2:
                            line_index = i
                    if line_index is None:
                        print("add")
                        fig.add_scatter3d(
                            name=line_trace_name1,
                            x=[x1, x2],
                            y=[y1, y2],
                            z=[z1, z2],
                            mode='lines',
                            line=dict(color='blue', width=w),
                            hoverinfo='text',  # Enable hover for the line
                            hovertext=f'Weight = {w/5}<br>Change in Severity = {y2-y1}<br>Change in Likelihood = {z2-z1}',
                            meta=w/5  # Store weight as metadata
                        )
                    clicked_points = []  # Reset clicked points
    return fig


if __name__ == '__main__':
    app.run_server(debug=True)
