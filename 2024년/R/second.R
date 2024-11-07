  # 다른 개발자가 만들어놓은 코드 모음
  # class에는 knn모델이 들어 있다.
  library(class)
  tr.idx <- c(1:25, 51:75, 101:125)
  ds.train <- iris[tr.idx, 1:4]
  ds.test <- iris[-tr.idx, 1:4]
  cl.train <- factor(iris[tr.idx, 5])
  cl.test <- factor(iris[-tr.idx, 5])
  
pred <- knn(ds.train, ds.test, cl.train, k=3, prob=TRUE)

pred

acc <- mean(pred==cl.test)
acc


