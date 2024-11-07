# iris 데이터셋의 첫 네 열 선택
mydate <- iris[, 1:4]

# k-means 클러스터링 수행, 클러스터 수를 3으로 설정
fit <- kmeans(x = mydate, centers = 3)
fit

# 클러스터 할당 결과
fit$cluster

# 클러스터 중심점 정보
fit$centers
