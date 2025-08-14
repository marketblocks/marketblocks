# What is marketblocks?

In September 2021, I took a deliberate career pause to focus on upskilling. With a degree in Mechanical Engineering and a professional background in Software Engineering, I recognised gaps in my understanding of fundamental computer science concepts and set out to close them through an intensive self-directed learning program.

Over the course of the year, I:
   - Studied data structures & algorithms, computer architecture, operating systems and networking using university-level textbooks.
   - Taught myself C++, with a focus on modern best practices — including RAII, smart pointers, move semantics, standard library usage, and clean, maintainable code.
   - Designed and built MarketBlocks — a high-performance cryptocurrency trading framework written in C++
      - Designed system architecture to run a "strategy" function against live or historical data, enabling both real-time trading and offline backtesting.
      - Implemented API clients for 5 major exchanges.
      - Built basic HTTP and Websocket clients.

Once the MVP of MarketBlocks was complete, I began applying for freelance work on Upwork and Fiverr to secure clients. I worked with several traders to design and implement custom trading strategies on top of the framework, delivering production-ready solutions tailored to their requirements.

This period strengthened my problem-solving skills, deepened my low-level programming expertise, and gave me real-world experience delivering software in both product and client-service contexts.




## The Product

<p></p>
<p align="center">
   <i><b>A single tool for the development, testing and deployment of your trading algorithm.</b></i>
</p>
<p></p>



marketblocks is a high performance C++ framework that reduces the complexity and development time of cryptocurrency trading algorithms. It provides an abstract interface for interacting with cryptocurrency exchanges and has built-in live test and back testing run modes which can be enabled simply through changing a config file. This allows trading strategies developed with marketblocks to be both exchange and run mode agnostic.

### Key Features
marketblocks boasts the following key features:
  - A consistent and predictable API for interacting with any exchange
  - Suppport for REST API endpoints and websocket streams
  - Live, Live-Test and Back-Test run modes
  - Easily extendable config file system
  - Built-in logging framework

### Supported Exchanges
marketblocks features built-in support for the following exchanges:
  - Kraken
  - Coinbase Pro
  - ByBit
  - Binance
  - Digifinex

### [Example 1](https://github.com/marketblocks/triangular_arbitrage_example) - (Triangular Arbitrage)

