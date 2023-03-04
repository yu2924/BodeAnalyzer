# BodeAnalyzer
 
 <img src="media/screenshot-ir.png" width="50%">

 ## What is this?

オーディオシステムの周波数特性を解析するためのシンプルなツールです。振幅と位相の応答を得るために、2種類の測定方法を用意しています。  
 A simple tool for analyzing the frequency response of audio systems. To obtain the amplitude and phase responses, two measurement method are provided.
 * Swept-Sine method, also known as TSP (Time-Stretched Pulse)
 * Stepped Sweep method

 Implemented with JUCE.

 ## Requirement

* JUCE framework: [download](https://juce.com/get-juce/download), [repository](https://github.com/juce-framework/JUCE)
* Projucer: [download](https://juce.com/discover/projucer), or build from source
* C++ build tools: Visual Studio, Xcode, etc.

 ## How to build

1. Open the .jucer file with the Projucer.
2. Correct the JUCE module path and properties, add exporters and save.
3. Build the generated C++ projects.

## Example

<img src="media/example schematic.svg" width="256">
<img src="media/example hardware.jpg" width="256">
<img src="media/example measurement.png" width="33%">

## Written by

[yu2924](https://twitter.com/yu2924)

## License

CC0 1.0 Universal

---

## Appendix

### The Latency Probe

この機能では、探査信号と応答信号の相互相関関数を求めることによってターゲットシステムを経由した往復レイテンシを特定します。探査信号として非周期的な乱数列を使用します。  
This function identifies the round-trip latency through the target system by determining the cross-correlation function between the probe signal and the response signal. An acyclic random number sequence is used as the probe signal.  

<img src="media/diagram-latency_probe.svg" width="90%">  

fig. Latency Probe processing flow diagram

<img src="media/corr-stimulus.png" width="25%">fig. random number sequence as plobe signal (a)

<img src="media/corr-response.png" width="25%">fig. delayed and filtered response signal (b)

<img src="media/corr-result.png" width="25%">fig. obtained correlation function (c)

### The Swept Sine method

この手法では、Swept Sineと呼ばれる刺激信号を使用してターゲットシステムのインパルス応答を求め、これを変換することで周波数応答を得ます。Swept SineはTSP (Time Stretched Pulse)としても知られています。  
The method uses a stimulus signal called the Swept Sine to obtain the impulse response of the target system, and then transforms it to obtain the frequency response. Swept Sine is also known as TSP (Time Stretched Pulse).  

<img src="media/diagram-swept_sine.svg" width="90%">  

fig. Swept Sine Method processing flow diagram

definitions:

$$N: \text{length of the entire stimulus signal}$$

$$m: \text{the pulse width constant,}\ \lt N/2$$

$$H'(n): \text{conjugate of}\ H(n)$$

$$j: \text{imaginary unit,}\ \sqrt{-1}$$

Linear TSP (OATSP):

$$
H(n)=
\begin{cases}
exp(\frac{-j 4 m \pi n^2}{N^2}), & \quad 0 \leq n \leq N/2\\
H'(N - n), & \quad N/2 < n < N, \text{conjugate symmetric}\\
\end{cases}
$$

Log TSP (Pink TSP):

$$
H(n)=
\begin{cases}
1, & \quad n = 0\\
\frac{exp(j \alpha n log(n))}{\sqrt{n}}, & \quad 0 < n \leq N/2\\
H'(N - n), & \quad N/2 < n < N, \text{conjugate symmetric}\\
\end{cases}
$$
where
$$
\alpha = \frac{2 m \pi}{((N / 2) log(N / 2))}
$$

Inverse Function:

$$
H^{-1}(n) = 1 / H(n)
$$

<img src="media/tsp-prerotation.png" width="25%">fig. linear tsp signal generated according to the formula

<img src="media/tsp-postrotation.png" width="25%">fig. linear tsp signal with rotation applied

<img src="media/ir-prerotation.png" width="25%">fig. obtained impulse response

<img src="media/ir-postrotation.png" width="25%">fig. impulse response with rotation applied

### The Stepped Sweep Method

この手法では、一ステップごとに刺激信号の周波数を変化させ、対応する振幅と位相の応答を逐次測定します。古典的な周波数応答の測定方法です。  
In this method, the frequency of the stimulus signal is varied at each step and the corresponding amplitude and phase responses are measured successively. It is a classical frequency response measurement method.  

<img src="media/diagram-stepped_sweep.svg" width="90%">  

fig. Stepped Sweep Method processing flow diagram
