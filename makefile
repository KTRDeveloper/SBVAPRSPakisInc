SRCS = $(shell find . -name "*.cpp" -not -path "./preprocess/archives/*") 

OBJS = $(addsuffix .o, $(basename $(SRCS)))

EXEC = PRSPakisInc

LIBS = -lkissat -Lsolvers/kissat-inc-psids/build \
	   -lm4ri -Lpreprocess/m4ri-20140914/.libs \
       -lpthread -pthread -lz -lm -lboost_thread -lboost_date_time -lboost_system -static

CXXFLAGS = -Isolvers/kissat-inc-psids \
		   -Ipreprocess/m4ri-20140914 \
		   -std=c++11 -O3

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(EXEC)
