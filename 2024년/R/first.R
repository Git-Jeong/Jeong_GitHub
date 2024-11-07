head(cars)
#head 함수의 경우 1번~6번 까지 출력하는 함수임

plot(dist~speed, data=cars)
#plot는 그래프를 그리는 함수임
# 상관관계 파악 후 회귀분석을 하는 것
#dist~speed 는 제공거리(dist)랑 속도(speed)의 그래프를 구하는 것


# 선형회귀 모델 개발
# lm 이게 리니어 모델로 선형모델
model <- lm(dist~speed, cars)

abline(model)

b <- coef(model)[1] # 인공지능에선 b(편향)이라고 부르고 회귀에선 y절편을 의미함 
w <- coef(model)[2] # ai에선 w(가중치)라고 하고, ㅇ겨기선 기울기라고 부름
#y = wx + b인 것

speed <- 45 #여기 x가 되는 것.
dist <- w*speed+b
dist


speed <- cars[,1]
pred <- w * speed + b
compare <- data.frame(pred, cars[,2], pred-cars[,2]) 
colnames(compare) <-c('예상', '실제', '오차')
head(compare)



