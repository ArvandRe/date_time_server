FROM gcc:10

ENV DATE_TIME_PORT=313
ENV TZ_DATABASE_PATH=/time_server/date_time_zones.csv

RUN apt-get update && \
    apt-get -y install python3-pip && \
    pip3 install conan && \
    apt-get -y install cmake
COPY . /time_server
WORKDIR /time_server/build
RUN conan install ..

RUN cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release && \
    cmake --build .

CMD ["./bin/time_server"] 
