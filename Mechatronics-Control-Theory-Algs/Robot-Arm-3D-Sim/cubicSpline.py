import csv

import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d

from mpl_toolkits.mplot3d import Axes3D
from matplotlib.patches import FancyArrowPatch
from mpl_toolkits.mplot3d import proj3d
from mpl_toolkits.mplot3d.art3d import Line3D



# This is a class to make arrows in 3D (I eventually settled with using segments, but I the code anyways)
class Arrow3D(FancyArrowPatch):
    def __init__(self, xs, ys, zs, *args, **kwargs):
        FancyArrowPatch.__init__(self, (0,0), (0,0), *args, **kwargs)
        self._verts3d = xs, ys, zs

    def do_3d_projection(self, renderer=None):
        xs3d, ys3d, zs3d = self._verts3d
        xs, ys, zs = proj3d.proj_transform(xs3d, ys3d, zs3d, self.axes.M)
        self.set_positions((xs[0],ys[0]),(xs[1],ys[1]))

        return np.min(zs)
    
    def set_position(self, x, y, z):
        self._verts3d = x, y, z

# Simple Util function to clam a value
def clamp(val, minval, maxval):
    val = minval if val < minval else val
    val = maxval if val > maxval else val 

    return val



# Does parametric Cubic Spline interpolation and returns the spline and vector coordinates for every multiple of delta from 0 <= t <= 1
def cubicInterpolate(xList, yList, zList, delta):
    # Create x, y, and z coords in numpy
    xList = np.array(xList)
    yList = np.array(yList)
    zList = np.array(zList)
    sizeX = xList.size
    sizeY = yList.size
    sizeZ = zList.size
    # Partition each coordinate as a vector associated with a time between 0 and 1
    t = np.linspace(0, 1, sizeX)
    r = np.vstack((xList.reshape((1, sizeX)), yList.reshape(1, sizeY), zList.reshape(1, sizeZ)))
    
    # Interpolate between these vectors
    spline = interp1d(t, r, kind='cubic')

    # Divvy up the time into some width of delta
    times = np.linspace(np.min(t), np.max(t), int(1 / delta))
    # Evaluate the spline at these times
    r = spline(times)

    # Return the spline and the values
    return [spline, r]


# Converts (x, y) into (angle1, angle2) of SCARA joints using inverse kinematics
def convertToAngularPosition(x, y, z, l1, l2):
    # First, find an angle2 that is used to find angle1
    angle2 = -np.arccos(clamp((x ** 2 + y ** 2 + z ** 2 - l1 ** 2 - l2 ** 2) / (2 * l1 * l2), -1, 1))
    # Find angle 1
    angle1 = np.arctan2(z, np.sqrt(x**2 + y ** 2)) - np.arctan2(l2 * np.sin(angle2), l1 + l2 * (np.cos(angle2)))
    # Then, add angle 1 to angle 2 so that we get the correct angle 2 as we defined it.
    angle2 += angle1
    # Finally, we get angle 3 from arctan.
    angle3 = np.arctan2(x, y)
    # Return the angle
    return [angle1, angle2, angle3]
    # for index, (xPoint, yPoint) in enumerate(zip(x, y)):


# Converts linear velocity into angular velocity using the inverse of the Jacobian matrix
def convertToAngularVelocity(linearVelocityVector, jacobianInverse):
    # (J^-1)(v)
    return np.dot(jacobianInverse, linearVelocityVector)


# Converts linear acceleration into angular acceleration using the derivative and inverse of the Jacobian matrix
def convertToAngularAcceleration(linearAccelerationVector, angularVelocityVector, jacobianInverse, jacobianDerivative):
    # (J^-1)(a - (J')(v))
    return np.dot(jacobianInverse, (np.atleast_2d(linearAccelerationVector).T - np.dot(jacobianDerivative, angularVelocityVector)))


# This takes the numeric derivative of the spline and returns a velocity vector
# TODO: find a function in some library to do this particular differentiation (make sure you can specify whether to not use the center formula for differentiation as the extremes of the path have no negative parameter t)
def numericDerivativeSpline(parameterizedSpline, number, delta):
    try:
        return (parameterizedSpline(number + delta) - parameterizedSpline(number)) / delta
    except:
        return (parameterizedSpline(number) - parameterizedSpline(number - (delta/2))) / delta


# This takes the second numeric derivative of the spline and returns an acceleration vector
# TODO: find a function in some library to do this particular differentiation (make sure you can specify whether to not use the center formula for differentiation as the extremes of the path have no negative parameter t)
def numericSecondDerivativeSpline(parameterizedSpline, number, deltaT, deltaV):
    try:
        return (numericDerivativeSpline(parameterizedSpline, number + deltaV, deltaT) - numericDerivativeSpline(
            parameterizedSpline, number, deltaT)) / deltaV
    except:
        return (numericDerivativeSpline(parameterizedSpline, number, deltaT) - numericDerivativeSpline(
            parameterizedSpline, number-(deltaV/2), deltaT)) / deltaV

# Compute the inverse of the Jacobian matrix for the system numerically.
def findJacobianInverse(angles, l1, l2):
    (theta1, theta2, theta3) = tuple(angles)
    
    # Define Jacobian
    jacobian = np.matrix([
        [-l1 * np.sin(theta3) * np.sin(theta1), -l2 * np.sin(theta3) * np.sin(theta2), np.cos(theta3) * (l2 * np.cos(theta2) + l1 * np.cos(theta1))],
        [-l1 * np.cos(theta3) * np.sin(theta1), -l2 * np.cos(theta3) * np.sin(theta2), -np.sin(theta3) * (l2 * np.cos(theta2) + l1 * np.cos(theta1))],
        [l1 * np.cos(theta1), l2 * np.cos(theta2), 0]])
    
    # Return Inverse
    return np.array(jacobian.I)


# Compute the derivative of the Jacobian Using a formula
def findJacobianDerivative(angles, anglesVelocity, l1, l2):
    (theta1, theta2, theta3) = tuple(angles)
    (theta1_dot, theta2_dot, theta3_dot) = tuple(map(lambda x: x[0], anglesVelocity))
    # Substitute for derivative formula
    jacobianDerivative = np.array([
        [
            -l1 * np.cos(theta3) * np.sin(theta1) * theta3_dot - l1 * np.sin(theta3) * np.cos(theta1) * theta1_dot, 
            -l2 * np.cos(theta3) * np.sin(theta2) * theta3_dot - l2 * np.sin(theta3) * np.cos(theta2) * theta2_dot,
            -l1 * np.cos(theta3) * np.sin(theta1) * theta1_dot - l2 * np.cos(theta3) * np.sin(theta2) * theta2_dot
                - np.sin(theta3) * (l1 * np.cos(theta1) + l2 * np.cos(theta2)) * theta3_dot],
        [
            l1 * np.sin(theta3) * np.sin(theta1) * theta3_dot - l1 * np.cos(theta3) * np.cos(theta1) * theta1_dot, 
            l2 * np.sin(theta3) * np.sin(theta2) * theta3_dot - l2 * np.cos(theta3) * np.cos(theta2) * theta2_dot,
            l1 * np.sin(theta3) * np.sin(theta1) * theta1_dot + l2 * np.sin(theta3) * np.sin(theta2) * theta2_dot
                - np.cos(theta3) * (l1 * np.cos(theta1) + l2 * np.cos(theta2)) * theta3_dot],
        [
            -l1 * np.sin(theta1) * theta1_dot, -l2 * np.sin(theta2) * theta2_dot, 0]])

    return jacobianDerivative


# Use Simpson's 1/3 rule to do some smexy numeric integration
# @param function - a parameterized function with 0 <= t <= 1
def computeArcLength(function, delta):
    arc_len = 0
    for i in range(int(1/delta)-1):
        current_norm = np.linalg.norm(function(i*delta))
        middle_norm = np.linalg.norm(function((i*delta + (i+1)*delta)/2))
        lookahead_norm = np.linalg.norm(function((i+1) * delta))
        arc_len += (delta/6) * (current_norm + 4*middle_norm + lookahead_norm)

    print(arc_len)
    return float(arc_len)

# First part of the piecewise of the acceleration profile
def accelProfilePart1(t, a, j, t_1, *args, **kwargs):
    return a*np.sin((j*t)/a)

# Third part of the piecewise of the acceleration profile
def accelProfilePart3(t, a, j, t_1, *args, **kwargs):
    return a*np.cos((j/a)*(t - (t_1+((np.pi*a)/(2*j)))))

# Fifth part of the piecewise of the acceleration profile
def accelProfilePart5(t, a, j, t_1, *args, **kwargs):
    return a*np.sin((j/a)*(t - ((2*t_1)-((2*np.pi*a)/j))))

# First part of the piecewise of the velocity profile
def velProfilePart1(t, a, j, t_1, *args, **kwargs):
    return ((a**2)/j)*(1-np.cos((j*t)/a))

# Second part of the piecewise of the velocity profile
def velProfilePart2(t, a, j, t_1, *args, **kwargs):
    return a*t + (1-(np.pi/2))*((a**2)/j)

# Third part of the piecewise of the velocity profile
def velProfilePart3(t, a, j, t_1, *args, **kwargs):
    return ((a**2)/j)*np.cos((j*t)/a - ((j*t_1)/a) - (np.pi/2)) + a*t_1 + ((a**2)/j)

# Fourth part of the piecewise of the velocity profile
def velProfilePart4(t, a, j, t_1, *args, **kwargs):
    return -a*t + 2*a*t_1 + (1+((3*np.pi)/2))*((a**2)/j)

# Fifth part of the piecewise of the velocity profile
def velProfilePart5(t, a, j, t_1, *args, **kwargs):
    return ((a**2)/j)*(1-np.cos((((j*t)/a)) - ((2*j*t_1)/a)))

# Here is the actual piecewise acceleration profile
def piecewiseAcceleration(time, a, j, t_1):
    if(0 <= time and time < (np.pi*a)/(2*j)):
        return accelProfilePart1(time, a, j, t_1)
    elif((np.pi*a)/(2*j) <= time and time < (np.pi*a)/(2*j) + t_1):
        return a
    elif((np.pi*a)/(2*j) + t_1 <= time and time < (3*np.pi*a)/(2*j) + t_1):
        return accelProfilePart3(time, a, j, t_1)
    elif((3*np.pi*a)/(2*j) + t_1 <= time and time < (3*np.pi*a)/(2*j) + 2*t_1):
        return -a
    elif((3*np.pi*a)/(2*j) + 2*t_1 <= time and time <= (2*np.pi*a)/(j) + 2*t_1):
       return accelProfilePart5(time, a, j, t_1)
    else:
        print("how?")
        return 0

# Here is the actual piecewise velocity profile
def piecewiseVelocity(time, a, j, t_1):
    if (0 <= time and time < (np.pi*a)/(2*j)):
        return velProfilePart1(time, a, j, t_1)
    elif((np.pi*a)/(2*j) <= time and time < (np.pi*a)/(2*j) + t_1):
        return velProfilePart2(time, a, j, t_1)
    elif((np.pi*a)/(2*j) + t_1 <= time and time < (3*np.pi*a)/(2*j) + t_1):
        return velProfilePart3(time, a, j, t_1)
    elif((3*np.pi*a)/(2*j) + t_1 <= time and time < (3*np.pi*a)/(2*j) + 2*t_1):
        return velProfilePart4(time, a, j, t_1)
    elif((3*np.pi*a)/(2*j) + 2*t_1 <= time and time <= (2*np.pi*a)/(j) + 2*t_1):
        return velProfilePart5(time, a, j, t_1)
    else:
        print("how?")
        return 0

# Compute the total time the arm will take to go through the motion
def computeTotalTime(a, j, L):
    t_1 = -(1 + (np.pi/2))*(a/j) + np.sqrt((1-np.pi+(((np.pi)**2)/4)) * ((a/j)**2) + L/a)
    t_total: int = ((2 * np.pi * a)/j) + 2*t_1

    return (t_total, t_1)


def computePVA(x, y, z, time, l1, l2, motion_bounds, fAndCoords, maxAcceleration, maxJerk):
    # Convert the cartesian position to angles the arm can go to reach position
    #print("coords:",x,y,z)

    t_1 = motion_bounds[1]
    t_total = motion_bounds[0]
    angles = convertToAngularPosition(x, y, z, l1, l2)
    #print("Theta:", angles)
    # Define J^-1
    jacobianInverse = findJacobianInverse(angles, l1, l2) 
    # Caculate the angular velocity vector based on the cartesian velocity vector of the interpolation
    linearVelocity = numericDerivativeSpline(fAndCoords[0], time, 1e-6)
    linearVelocity *= piecewiseVelocity(time*t_total, maxAcceleration, maxJerk, t_1) /np.linalg.norm(linearVelocity)
    anglesVelocity = np.vstack(
        convertToAngularVelocity(linearVelocity, jacobianInverse))
    

    # Define J'
    jacobianDerivative = findJacobianDerivative(angles, anglesVelocity, l1, l2)


    # Caculate the angular acceleration vector based on the cartesian acceleration vector of the interpolation
    linearAcceleration = numericSecondDerivativeSpline(fAndCoords[0], time, 1e-6, 1e-6)
    linearAcceleration *= piecewiseAcceleration(time*t_total, maxAcceleration, maxJerk, t_1)/np.linalg.norm(linearAcceleration)

    anglesAcceleration = np.vstack(
        convertToAngularAcceleration(linearAcceleration,
                                        anglesVelocity, jacobianInverse, jacobianDerivative))
    
    # Define one line of the profile in the format (angle1, angle2, angle1Velocity, angle2Velocity, angle1Acceleration, angle2Acceleration)

    return (
        [angles[0], angles[1], angles[2], anglesVelocity[0][0], anglesVelocity[1][0], anglesVelocity[2][0], anglesAcceleration[0][0], anglesAcceleration[1][0], anglesAcceleration[2][0]])

# Util function for a unit vector
def unit_vector(vector):
    return vector / np.linalg.norm(vector)

# Util function to get the angle between two vectors
def angle_between(v1, v2):
    v1_u = unit_vector(v1)
    v2_u = unit_vector(v2)
    return np.arccos(np.clip(np.dot(v1_u, v2_u), -1.0, 1.0))

# Find the closest coordinate on the spline given a specific vector.
def findClosestCoord(coords, thetas, l1, l2, function, delta):
    (theta1, theta2, theta3) = thetas
         
    angleToPosVector = np.array(direct_kinematics(theta1, theta2, theta3, l1, l2)[1])
    closest_distance = np.linalg.norm(np.array(coords[0]) - angleToPosVector)
    closest_coord_index = 0
    for i, coord in enumerate(coords):
        posVector = np.array(coord)

        difference = posVector - angleToPosVector

        velocity = numericDerivativeSpline(function, i*delta, 1e-6)

        if(np.linalg.norm(difference) < closest_distance and angle_between(velocity.T, difference) < 0.6):
            closest_distance = np.linalg.norm(difference)
            closest_coord_index = i

    return closest_coord_index





# Create a motion profile in the form of a 2D array, with each column in the array specifying an angular positon, velocity, or acceleration for every deltaX in the movement
def createProfile(knotPointsX, knotPointsY, knotPointsZ, delta, l1, l2, maxAcceleration=2, maxJerk=2, interpolate=False):
    motionProfile = []
    # First, define the path using the interpolation
    fAndCoords = cubicInterpolate(knotPointsX, knotPointsY, knotPointsZ, delta)

    # Find our arc length
    arc_length = computeArcLength(fAndCoords[0], delta)

    motion_bounds = computeTotalTime(maxAcceleration, maxJerk, arc_length)


    coords = list(zip(fAndCoords[1][0, :], fAndCoords[1][1, :], fAndCoords[1][2, :]))
    # Iterate through the x, y, and z coordinates, specifying which multiple of t you are on.
    wait_time = 0
    for i, (x, y, z) in enumerate(coords):

        if(wait_time > 0):
            wait_time -= 1
            continue
        time = i * delta
        profileComponent = computePVA(x, y, z, time, l1, l2, motion_bounds, fAndCoords, maxAcceleration, maxJerk)
        motionProfile.append(profileComponent)

        # NOTE This code snippet "beautifies" the animation.
        # The given animation can render without interpolation, but the
        # amount of sections that can reasonably be rendered by the animation is limited by runtime.
        # A small amount of sections make the animation choppy.
        if(interpolate and not(i+1 >= len(fAndCoords[1][0, :]))):

            thetas = convertToAngularPosition(x, y, z, l1, l2)

            print(thetas)
            lastcomponent = motionProfile[-1]
            
            for j, theta in enumerate(thetas):
                thetas[j] = theta + (lastcomponent[3+j] * 0.05)

            print(thetas)
            next_coord_index = findClosestCoord(coords, thetas, l1, l2, fAndCoords[0], delta)
            print(next_coord_index, i)
            wait_time = next_coord_index - i
            print(wait_time)


        # Debug
        # Add the line to the profile
        
    # Return profile
    print(fAndCoords[0])
    print(computeTotalTime(maxAcceleration, maxJerk, arc_length))
    return motionProfile


# # Convert the motion profile generated from the above funtion from being measured in radians to the position being meausred in centidegrees, and the other two values being measured in degrees
# def convertToDegreeSystem(profileComponent):
#     return [profileComponent[0] * (18000 / np.pi), profileComponent[1] * (18000 / np.pi),
#             profileComponent[2][0] * (180 / np.pi), profileComponent[3][0] * (180 / np.pi),
#             profileComponent[4][0] * (180 / np.pi), profileComponent[5][0] * (180 / np.pi)]


# # Convert the motion profile generated from createProfile from being measured in radians starting at the x axis to being meausured in the robot's local coordinate system
# def convertToRobotCoordinateSystem(profileComponent):
#     return convertToDegreeSystem(
#         [-(profileComponent[0]) + np.pi / 2, -(profileComponent[1]), -profileComponent[2], -profileComponent[3],
#          -profileComponent[4], -profileComponent[5]])

# Reconverts the data in the profile into an (x, y, z) coordinate that the animation can render.
def direct_kinematics(theta1, theta2, theta3, l1, l2):

    return ((
        np.sin(theta3) * l1*np.cos(theta1),
        np.cos(theta3)* l1*np.cos(theta1),
        l1*np.sin(theta1)
    ), (
        np.sin(theta3)*(l2*np.cos(theta2) + l1*np.cos(theta1)),
        np.cos(theta3)*(l2*np.cos(theta2) + l1*np.cos(theta1)),
        l1*np.sin(theta1) + l2*np.sin(theta2)
    ))






knotX = [-1, 2, 4, 5, -4, -4, -5, -8]
knotY = [7, 7, 5, 8, 4, -2, -6, -2]
knotZ = [-5, -4, -3, 2, 0, -1, -2, -3]

# Creates a profile and animates it.
arm1Length = 6
arm2Length = 6
profile = createProfile(knotX, knotY, knotZ, 0.01, arm1Length, arm2Length, maxAcceleration=3, maxJerk=5, interpolate=True)
fig = plt.figure(figsize=(8, 8))
ax = plt.axes(projection='3d')

xyzvectors = cubicInterpolate(knotX, knotY, knotZ,  0.001)[1]
ax.plot(xyzvectors[0,:], xyzvectors[1,:], xyzvectors[2,:])

ax.set_xlim(-20, 20)
ax.set_ylim(-20, 20)
ax.set_zlim(-20, 20)

ax.set_xlabel("X Axis")
ax.set_ylabel("Y Axis")
ax.set_zlabel("Z Axis")


theta1, theta2, theta3 = profile[0][0], profile[0][1], profile[0][2]

# Some test code
a = convertToAngularPosition(0.2221095887179398, -0.763246952448105, -0.2977610870247407, arm1Length, arm2Length)
print(a)
#print(theta1, theta2, theta3)
#print(direct_kinematics(a[0], a[1], a[1], arm1Length, arm2Length))

directions = direct_kinematics(a[0], a[1], a[2], arm1Length, arm2Length)


print(direct_kinematics(2, 5, 6, 6, 6)[1])

u1, v1, w1 = directions[0]
u2, v2, w2 = directions[1]

print("Position 1:", u1, v1, w1)
print("Position 2:", u2, v2, w2)

arm1magnitude = np.sqrt(u1**2 + v1**2 + w1**2)
arm2magnitude = np.sqrt((u2-u1)**2 + (v2-v1)**2 + (w2-w1)**2)

print(arm1magnitude, arm2magnitude)

arm1 = ax.plot([0, u1], [0, v1], [0, w1], color='r')[0]
arm2 = ax.plot([u1, u2], [v1, v2], [w1, w2], color='g')[0]

# arm1 = Arrow3D([0, u1], [0, v1], [0, w1], color='red', arrowstyle="-|>")

#frameNumber = ax.text(0.05, 0.95, '', horizontalalignment='left', verticalalignment='top', transform=ax.transAxes)\


#plt.show()


# This function exports a profile that is readable by the SCARA arm into a csv file
def exportProfile(profile_, profileName):
    with open(profileName + ".csv", "w+") as file:
        writer = csv.writer(file, delimiter=',', quotechar='|', lineterminator="\n", quoting=csv.QUOTE_MINIMAL)
        for i in profile_:
            writer.writerow(i)


exportProfile(profile, "myFirstProfile")


def animate(num: int, arm1: Line3D, arm2: Line3D):
    directions = direct_kinematics(profile[num][0], 
                                   profile[num][1], 
                                   profile[num][2],
                                   arm1Length, 
                                   arm2Length)
    u1, v1, w1 = directions[0]
    u2, v2, w2 = directions[1]
    
    arm1.set_data_3d([0, u1], [0, v1], [0, w1])
    arm2.set_data_3d([u1, u2], [v1, v2], [w1, w2])

    return [arm1, arm2]


anim = animation.FuncAnimation(fig, animate, fargs=(arm1, arm2), frames=len(profile), interval=50, blit=False, repeat=True)
anim.save("arm.gif")
plt.show()

