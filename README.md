# cellulite
This is a sample app made while poking at the Cinder 0.9.0 framework. It's a simple particle system that constantly rearranges the particles in randomized and fixed shapes.

Here's a video of some example output: https://www.youtube.com/watch?v=qNN0GOtMXoQ

## compiling
1. Download Cinder 0.9.0 for Visual Studio 2013:<br>
https://libcinder.org/static/releases/cinder_0.9.0_vc2013.zip<br>
(the code contains some windows-only pieces right now)

2. Add an environment variable named CINDER_090_2013 that points to the extracted Cinder folder.

3. And of course, pull and compile this app.

## design
I had two main things I wanted to explore with for this app. On the technical side, I wanted to play with instancing for drawing lots of particles. On the artistic side, I wanted to create a scene that appeared flat but used motion to create a sense of depth (which is why I've only linked a video, still shots don't really look like anything).

In terms of the number of particles, realistically, I'm not really making much use of instancing. It's currently drawing I think 10,000 particles, but I did an earlier experiment that was drawing 180,000 without issue. One thing worth noting is that the rendering is not limited to the number of instanced items on the GPU. Instead, if there are more particles in the app than instances allocated on the GPU, during a single frame the same GPU buffer is reused as much as necessary. This makes no sense to me, I would assume the actual GPU rendering happens asynchronously from the main app thread, so whatever you assign to the buffer might not get used right away, but in practice I've never seen a single glitch doing this technique (and I've pushed it, rendering 10,000 particles into a buffer of size 256). So that's interesting to me.

To achieve the flat look, the scene is rendered into an FBO which is in turn rendered to the screen with a shader that saturates and then clamps the transparency value of each particle. This lets multiple particles blur together without looking like a bunch of overlapping particles. I think probably a better way to achieve this would be to just use a blur effect to create a drop shadow, and then render the scene separately at a lower transparency, but I haven't tried that yet.

## code
The code is in two main pieces: Everything in the *src/kt/* folder has been pulled from a generic library for application development I've written. Everything in the main *src/* folder is app-specific.

The app is intended to have a very large number of particles, so describing how they move is done mostly in a separate thread. The main pieces are:
* **Particles** store a bezier curve describing how they move.
* **Generators** create new beziers for each particle. There are multiple kinds of generators, the main ones (as seen in the video) being one that is completely random, one that creates random lines the particles attract to, and one that assigns particle information based on an image file.
* The **feeder** is responsible for generating new frames of particle data. It manages a thread that runs a generator to create a new frame each time a client grabs the current frame.
* The **particle render** is responsible for performing the actual drawing of particles -- feed it a particle list, and it renders it via a ci::gl::Batch.
* The **particle view** is responsible for main-thread updating of the particle positions, which mostly means setting each particle to the correct position on its curve based on the current time.
* The **cs app** sets everything up and ties it together.

## license
There is no license on this code -- do whatever you like -- but I have no responsibility for any errors or damage that results from using it.

## contact
Questions or comments, contact me at hackborn@gmail.com
