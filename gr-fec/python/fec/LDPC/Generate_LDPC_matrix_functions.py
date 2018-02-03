#!/usr/bin/env python
#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import string, sys
from numpy import *
from numpy.random import shuffle, randint
from numpy.linalg import inv, det

# 0 gives no debug output, 1 gives a little, 2 gives a lot
#verbose = 1 #######################################################

def read_alist_file(filename):
  """
  This function reads in an alist file and creates the
  corresponding parity check matrix H. The format of alist
  files is described at:
  http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
  """

  myfile    = open(filename,'r')
  data      = myfile.readlines()
  size      = string.split(data[0])
  numCols   = int(size[0])
  numRows   = int(size[1])
  H = zeros((numRows,numCols))
  for lineNumber in arange(4,4+numCols):
    indices = string.split(data[lineNumber])
    for index in indices:
      H[int(index)-1,lineNumber-4] = 1
  # The subsequent lines in the file list the indices for where
  # the 1s are in the rows, but this is redundant
  # information.

  return H

def write_alist_file(filename, H, verbose=0):
  """
  This function writes an alist file for the parity check
  matrix. The format of alist files is described at:
  http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
  """

  try:
    myfile = open(filename,'w')
  except:
    sys.stderr.write("Could not open output file '{0}'".format(filename))
    sys.exit(1)

  numRows = H.shape[0]
  numCols = H.shape[1]

  tempstring = `numCols` + ' ' + `numRows` + '\n'
  myfile.write(tempstring)

  tempstring1 = ''
  tempstring2 = ''
  maxRowWeight = 0
  for rowNum in arange(numRows):
    nonzeros = array(H[rowNum,:].nonzero())
    rowWeight = nonzeros.shape[1]
    if rowWeight > maxRowWeight:
      maxRowWeight = rowWeight
    tempstring1 = tempstring1 + `rowWeight` + ' '
    for tempArray in nonzeros:
      for index in tempArray:
        tempstring2 = tempstring2 + `index+1` + ' '
      tempstring2 = tempstring2 + '\n'
  tempstring1 = tempstring1 + '\n'

  tempstring3 = ''
  tempstring4 = ''
  maxColWeight = 0
  for colNum in arange(numCols):
    nonzeros = array(H[:,colNum].nonzero())
    colWeight = nonzeros.shape[1]
    if colWeight > maxColWeight:
      maxColWeight = colWeight
    tempstring3 = tempstring3 + `colWeight` + ' '
    for tempArray in nonzeros:
      for index in tempArray:
        tempstring4 = tempstring4 + `index+1` + ' '
      tempstring4 = tempstring4 + '\n'
  tempstring3 = tempstring3 + '\n'

  tempstring = `maxColWeight` + ' ' + `maxRowWeight` + '\n'
  # write out max column and row weights
  myfile.write(tempstring)
  # write out all of the column weights
  myfile.write(tempstring3)
  # write out all of the row weights
  myfile.write(tempstring1)
  # write out the nonzero indices for each column
  myfile.write(tempstring4)
  # write out the nonzero indices for each row
  myfile.write(tempstring2)
  # close the file
  myfile.close()


class LDPC_matrix:
  """ Class for a LDPC parity check matrix """
  def __init__(self, alist_filename = None,
             n_p_q = None,
             H_matrix = None):
    if (alist_filename != None):
      self.H = self.read_alist_file(alist_filename)
    elif (n_p_q != None):
      self.H = self.regular_LDPC_code_contructor(n_p_q)
    elif (H_matrix != None):
      self.H = H_matrix
    else:
      print 'Error: provide either an alist filename,',
      print 'parameters for constructing regular LDPC parity',
      print 'check matrix, or a numpy array.'

    self.rank = linalg.matrix_rank(self.H)
    self.numRows = self.H.shape[0]
    self.n = self.H.shape[1]
    self.k = self.n -self.numRows

  def regular_LDPC_code_contructor(self,n_p_q):
    """
    This function constructs a LDPC parity check matrix
    H. The algorithm follows Gallager's approach where we create
    p submatrices and stack them together. Reference: Turbo
    Coding for Satellite and Wireless Communications, section
    9,3.

    Note: the matrices computed from this algorithm will never
    have full rank. (Reference Gallager's Dissertation.) They
    will have rank = (number of rows - p + 1). To convert it
    to full rank, use the function get_full_rank_H_matrix
    """

    n = n_p_q[0]  # codeword length
    p = n_p_q[1]  # column weight
    q = n_p_q[2]  # row weight
    # TODO: There should probably be other guidelines for n/p/q,
    # but I have not found any specifics in the literature....

    # For this algorithm, n/p must be an integer, because the
    # number of rows in each submatrix must be a whole number.
    ratioTest = (n*1.0)/q
    if ratioTest%1 != 0:
      print '\nError in regular_LDPC_code_contructor: The'
      print 'ratio of inputs n/q must be a whole number.\n'
      return

    # First submatrix first:
    m = (n*p)/q  # number of rows in H matrix
    submatrix1 = zeros((m/p,n))
    for row in arange(m/p):
      range1 = row*q
      range2 = (row+1)*q
      submatrix1[row,range1:range2] = 1
    H = submatrix1

    # Create the other submatrices and vertically stack them on.
    submatrixNum = 2
    newColumnOrder = arange(n)
    while submatrixNum <= p:
      submatrix = zeros((m/p,n))
      shuffle(newColumnOrder)

      for columnNum in arange(n):
        submatrix[:,columnNum] = \
                      submatrix1[:,newColumnOrder[columnNum]]

      H = vstack((H,submatrix))
      submatrixNum = submatrixNum + 1

    # Double check the row weight and column weights.
    size = H.shape
    rows = size[0]
    cols = size[1]

    # Check the row weights.
    for rowNum in arange(rows):
      nonzeros = array(H[rowNum,:].nonzero())
      if nonzeros.shape[1] != q:
        print 'Row', rowNum, 'has incorrect weight!'
        return

    # Check the column weights
    for columnNum in arange(cols):
      nonzeros = array(H[:,columnNum].nonzero())
      if nonzeros.shape[1] != p:
        print 'Row', columnNum, 'has incorrect weight!'
        return

    return H

def greedy_upper_triangulation(H, verbose=0):
  """
  This function performs row/column permutations to bring
  H into approximate upper triangular form via greedy
  upper triangulation method outlined in Modern Coding
  Theory Appendix 1, Section A.2
  """
  H_t  = H.copy()

  # Per email from Dr. Urbanke, author of this textbook, this
  # algorithm requires H to be full rank
  if linalg.matrix_rank(H_t) != H_t.shape[0]:
      print 'Rank of H:', linalg.matrix_rank(tempArray)
      print 'H has', H_t.shape[0], 'rows'
      print 'Error: H must be full rank.'
      return

  size = H_t.shape
  n = size[1]
  k = n - size[0]
  g = t = 0

  while t != (n-k-g):
    H_residual = H_t[t:n-k-g,t:n]
    size       = H_residual.shape
    numRows    = size[0]
    numCols    = size[1]

    minResidualDegrees = zeros((1,numCols))

    for colNum in arange(numCols):
      nonZeroElements = array(H_residual[:,colNum].nonzero())
      minResidualDegrees[0,colNum] = nonZeroElements.shape[1]

    # Find the minimum nonzero residual degree
    nonZeroElementIndices = minResidualDegrees.nonzero()
    nonZeroElements = minResidualDegrees[nonZeroElementIndices[0],
                                         nonZeroElementIndices[1]]
    minimumResidualDegree = nonZeroElements.min()

    # Get indices of all of the columns in H_t that have degree
    # equal to the min positive residual degree, then pick a
    # random column c.
    indices = (minResidualDegrees == minimumResidualDegree)\
                                     .nonzero()[1]
    indices = indices + t
    if indices.shape[0] == 1:
      columnC = indices[0]
    else:
      randomIndex = randint(0,indices.shape[0],(1,1))[0][0]
      columnC = indices[randomIndex]

    Htemp = H_t.copy()

    if minimumResidualDegree == 1:
      # This is the 'extend' case
      rowThatContainsNonZero = H_residual[:,columnC-t].nonzero()[0][0]

      # Swap column c with column t. (Book says t+1 but we
      # index from 0, not 1.)
      Htemp[:,columnC] = H_t[:,t]
      Htemp[:,t] = H_t[:,columnC]
      H_t = Htemp.copy()
      Htemp = H_t.copy()
      # Swap row r with row t. (Book says t+1 but we index from
      # 0, not 1.)
      Htemp[rowThatContainsNonZero + t,:] = H_t[t,:]
      Htemp[t,:] = H_t[rowThatContainsNonZero + t,:]
      H_t = Htemp.copy()
      Htemp = H_t.copy()
    else:
      # This is the 'choose' case.
      rowsThatContainNonZeros = H_residual[:,columnC-t]\
                                          .nonzero()[0]

      # Swap column c with column t. (Book says t+1 but we
      # index from 0, not 1.)
      Htemp[:,columnC] = H_t[:,t]
      Htemp[:,t] = H_t[:,columnC]
      H_t = Htemp.copy()
      Htemp = H_t.copy()

      # Swap row r1 with row t
      r1 = rowsThatContainNonZeros[0]
      Htemp[r1+t,:] = H_t[t,:]
      Htemp[t,:] = H_t[r1+t,:]
      numRowsLeft = rowsThatContainNonZeros.shape[0]-1
      H_t = Htemp.copy()
      Htemp = H_t.copy()

      # Move the other rows that contain nonZero entries to the
      # bottom of the matrix. We can't just swap them,
      # otherwise we will be pulling up rows that we pushed
      # down before. So, use a rotation method.
      for index in arange (1,numRowsLeft+1):
        rowInH_residual = rowsThatContainNonZeros[index]
        rowInH_t = rowInH_residual + t - index +1
        m = n-k
        # Move the row with the nonzero element to the
        # bottom; don't update H_t.
        Htemp[m-1,:] = H_t[rowInH_t,:]
        # Now rotate the bottom rows up.
        sub_index = 1
        while sub_index < (m - rowInH_t):
          Htemp[m-sub_index-1,:] = H_t[m-sub_index,:]
          sub_index = sub_index+1
        H_t = Htemp.copy()
        Htemp = H_t.copy()

      # Save temp H as new H_t.
      H_t = Htemp.copy()
      Htemp = H_t.copy()
      g = g + (minimumResidualDegree - 1)

    t = t + 1

  if g == 0:
    if verbose:
      print 'Error: gap is 0.'
    return

  # We need to ensure phi is nonsingular.
  T = H_t[0:t, 0:t]
  E = H_t[t:t+g,0:t]
  A = H_t[0:t,t:t+g]
  C = H_t[t:t+g,t:t+g]
  D = H_t[t:t+g,t+g:n]

  invTmod2array = inv_mod2(T)
  temp1  = dot(E,invTmod2array) % 2
  temp2  = dot(temp1,A) % 2
  phi    = (C - temp2) % 2
  if phi.any():
    try:
      # Try to take the inverse of phi.
      invPhi = inv_mod2(phi)
    except linalg.linalg.LinAlgError:
      # Phi is singular
      if verbose > 1:
        print 'Initial phi is singular'
    else:
      # Phi is nonsingular, so we need to use this version of H.
      if verbose > 1:
        print 'Initial phi is nonsingular'
      return [H_t, g, t]
  else:
    if verbose:
      print 'Initial phi is all zeros:\n', phi

  # If the C and D submatrices are all zeros, there is no point in
  # shuffling them around in an attempt to find a good phi.
  if not (C.any() or D.any()):
    if verbose:
      print 'C and D are all zeros. There is no hope in',
      print 'finding a nonsingular phi matrix. '
    return

  # We can't look at every row/column permutation possibility
  # because there would be (n-t)! column shuffles and g! row
  # shuffles. g has gotten up to 12 in tests, so 12! would still
  # take quite some time. Instead, we will just pick an arbitrary
  # number of max iterations to perform, then break.
  maxIterations = 300
  iterationCount = 0
  columnsToShuffle = arange(t,n)
  rowsToShuffle = arange(t,t+g)

  while iterationCount < maxIterations:
    if verbose > 1:
      print 'iterationCount:', iterationCount
    tempH = H_t.copy()

    shuffle(columnsToShuffle)
    shuffle(rowsToShuffle)
    index = 0
    for newDestinationColumnNumber in arange(t,n):
      oldColumnNumber = columnsToShuffle[index]
      tempH[:,newDestinationColumnNumber] = \
                                         H_t[:,oldColumnNumber]
      index +=1

    tempH2 = tempH.copy()
    index = 0
    for newDesinationRowNumber in arange(t,t+g):
      oldRowNumber = rowsToShuffle[index]
      tempH[newDesinationRowNumber,:] = tempH2[oldRowNumber,:]
      index +=1

    # Now test this new H matrix.
    H_t = tempH.copy()
    T = H_t[0:t, 0:t]
    E = H_t[t:t+g,0:t]
    A = H_t[0:t,t:t+g]
    C = H_t[t:t+g,t:t+g]
    invTmod2array = inv_mod2(T)
    temp1  = dot(E,invTmod2array) % 2
    temp2  = dot(temp1,A) % 2
    phi    = (C - temp2) % 2
    if phi.any():
      try:
        # Try to take the inverse of phi.
        invPhi = inv_mod2(phi)
      except linalg.linalg.LinAlgError:
        # Phi is singular
        if verbose > 1:
          print 'Phi is still singular'
      else:
        # Phi is nonsingular, so we're done.
        if verbose:
          print 'Found a nonsingular phi on',
          print 'iterationCount = ', iterationCount
        return [H_t, g, t]
    else:
      if verbose > 1:
        print 'phi is all zeros'

    iterationCount +=1

  # If we've reached this point, then we haven't found a
  # version of H that has a nonsingular phi.
  if verbose:
    print '--- Error: nonsingular phi matrix not found.'

def inv_mod2(squareMatrix, verbose=0):
  """
  Calculates the mod 2 inverse of a matrix.
  """
  A = squareMatrix.copy()
  t = A.shape[0]

  # Special case for one element array [1]
  if A.size == 1 and A[0] == 1:
    return array([1])

  Ainverse = inv(A)
  B = det(A)*Ainverse
  C = B % 2

  # Encountered lots of rounding errors with this function.
  # Previously tried floor, C.astype(int), and casting with (int)
  # and none of that works correctly, so doing it the tedious way.

  test = dot(A,C) % 2
  tempTest = zeros_like(test)
  for colNum in arange(test.shape[1]):
    for rowNum in arange(test.shape[0]):
      value = test[rowNum,colNum]
      if (abs(1-value)) < 0.01:
        # this is a 1
        tempTest[rowNum,colNum] = 1
      elif (abs(2-value)) < 0.01:
        # there shouldn't be any 2s after B % 2, but I'm
        # seeing them!
        tempTest[rowNum,colNum] = 0
      elif (abs(0-value)) < 0.01:
        # this is a 0
        tempTest[rowNum,colNum] = 0
      else:
        if verbose > 1:
          print 'In inv_mod2. Rounding error on this',
          print 'value? Mod 2 has already been done.',
          print 'value:', value

  test = tempTest.copy()

  if (test - eye(t,t) % 2).any():
    if verbose:
      print 'Error in inv_mod2: did not find inverse.'
    # TODO is this the most appropriate error to raise?
    raise linalg.linalg.LinAlgError
  else:
    return C

def swap_columns(a,b,arrayIn):
  """
  Swaps two columns in a matrix.
  """
  arrayOut = arrayIn.copy()
  arrayOut[:,a] = arrayIn[:,b]
  arrayOut[:,b] = arrayIn[:,a]
  return arrayOut

def move_row_to_bottom(i,arrayIn):
  """"
  Moves a specified row (just one) to the bottom of the matrix,
  then rotates the rows at the bottom up.

  For example, if we had a matrix with 5 rows, and we wanted to
  push row 2 to the bottom, then the resulting row order would be:
  1,3,4,5,2
  """
  arrayOut = arrayIn.copy()
  numRows = arrayOut.shape[0]
  # Push the specified row to the bottom.
  arrayOut[numRows-1] = arrayIn[i,:]
  # Now rotate the bottom rows up.
  index = 2
  while (numRows-index) >= i:
    arrayOut[numRows-index,:] = arrayIn[numRows-index+1]
    index = index + 1
  return arrayOut

def get_full_rank_H_matrix(H, verbose=False):
  """
  This function accepts a parity check matrix H and, if it is not
  already full rank, will determine which rows are dependent and
  remove them. The updated matrix will be returned.
  """
  tempArray = H.copy()
  if linalg.matrix_rank(tempArray) == tempArray.shape[0]:
    if verbose:
      print 'Returning H; it is already full rank.'
    return tempArray

  numRows = tempArray.shape[0]
  numColumns = tempArray.shape[1]
  limit = numRows
  rank = 0
  i = 0

  # Create an array to save the column permutations.
  columnOrder = arange(numColumns).reshape(1,numColumns)

  # Create an array to save the row permutations. We just need
  # this to know which dependent rows to delete.
  rowOrder = arange(numRows).reshape(numRows,1)

  while i < limit:
    if verbose:
      print 'In get_full_rank_H_matrix; i:', i
    # Flag indicating that the row contains a non-zero entry
    found  = False
    for j in arange(i, numColumns):
      if tempArray[i, j] == 1:
        # Encountered a non-zero entry at (i, j)
        found =  True
        # Increment rank by 1
        rank = rank + 1
        # Make the entry at (i,i) be 1
        tempArray = swap_columns(j,i,tempArray)
        # Keep track of the column swapping
        columnOrder = swap_columns(j,i,columnOrder)
        break
    if found == True:
      for k in arange(0,numRows):
        if k == i: continue
        # Checking for 1's
        if tempArray[k, i] == 1:
          # Add row i to row k
          tempArray[k,:] = tempArray[k,:] + tempArray[i,:]
          # Addition is mod2
          tempArray = tempArray.copy() % 2
          # All the entries above & below (i, i) are now 0
      i = i + 1
    if found == False:
      # Push the row of 0s to the bottom, and move the bottom
      # rows up (sort of a rotation thing).
      tempArray = move_row_to_bottom(i,tempArray)
      # Decrease limit since we just found a row of 0s
      limit -= 1
      # Keep track of row swapping
      rowOrder = move_row_to_bottom(i,rowOrder)

  # Don't need the dependent rows
  finalRowOrder = rowOrder[0:i]

  # Reorder H, per the permutations taken above .
  # First, put rows in order, omitting the dependent rows.
  newNumberOfRowsForH = finalRowOrder.shape[0]
  newH = zeros((newNumberOfRowsForH, numColumns))
  for index in arange(newNumberOfRowsForH):
    newH[index,:] = H[finalRowOrder[index],:]

  # Next, put the columns in order.
  tempHarray = newH.copy()
  for index in arange(numColumns):
    newH[:,index] = tempHarray[:,columnOrder[0,index]]

  if verbose:
    print 'original H.shape:', H.shape
    print 'newH.shape:', newH.shape

  return newH

def get_best_matrix(H, numIterations=100, verbose=False):
  """
  This function will run the Greedy Upper Triangulation algorithm
  for numIterations times, looking for the lowest possible gap.
  The submatrices returned are those needed for real-time encoding.
  """

  hadFirstJoy = 0
  index = 1
  while index <= numIterations:
    if verbose:
      print '--- In get_best_matrix, iteration:', index
    index += 1
    try:
      ret = greedy_upper_triangulation(H, verbose)
    except ValueError, e:
      if verbose > 1:
        print 'greedy_upper_triangulation error: ', e
    else:
      if ret:
        [betterH, gap, t] = ret
      else:
        continue

      if not hadFirstJoy:
        hadFirstJoy = 1
        bestGap = gap
        bestH = betterH.copy()
        bestT = t
      elif gap < bestGap:
        bestGap = gap
        bestH = betterH.copy()
        bestT = t


  if hadFirstJoy:
    return [bestH, bestGap]
  else:
    if verbose:
      print 'Error: Could not find appropriate H form',
      print 'for encoding.'
    return

def getSystematicGmatrix(GenMatrix):
  """
  This function finds the systematic form of the generator
  matrix GenMatrix. This form is G = [I P] where I is an identity
  matrix and P is the parity submatrix. If the GenMatrix matrix
  provided is not full rank, then dependent rows will be deleted.

  This function does not convert parity check (H) matrices to the 
  generator matrix format. Use the function getSystematicGmatrixFromH
  for that purpose.
  """
  tempArray = GenMatrix.copy()
  numRows = tempArray.shape[0]
  numColumns = tempArray.shape[1]
  limit = numRows
  rank = 0
  i = 0
  while i < limit:
    # Flag indicating that the row contains a non-zero entry
    found = False
    for j in arange(i, numColumns):
      if tempArray[i, j] == 1:
        # Encountered a non-zero entry at (i, j)
        found = True
        # Increment rank by 1
        rank = rank + 1
        # make the entry at (i,i) be 1
        tempArray = swap_columns(j,i,tempArray)
        break
    if found == True:
      for k in arange(0,numRows):
        if k == i: continue
        # Checking for 1's
        if tempArray[k, i] == 1:
          # add row i to row k
          tempArray[k,:] = tempArray[k,:] + tempArray[i,:]
          # Addition is mod2
          tempArray = tempArray.copy() % 2
          # All the entries above & below (i, i) are now 0
      i = i + 1
    if found == False:
      # push the row of 0s to the bottom, and move the bottom
      # rows up (sort of a rotation thing)
      tempArray = move_row_to_bottom(i,tempArray)
      # decrease limit since we just found a row of 0s
      limit -= 1
  # the rows below i are the dependent rows, which we discard
  G = tempArray[0:i,:]
  return G

def getSystematicGmatrixFromH(H, verbose=False):
  """
  If given a parity check matrix H, this function returns a
  generator matrix G in the systematic form: G = [I P]
    where:  I is an identity matrix, size k x k
            P is the parity submatrix, size k x (n-k)
  If the H matrix provided is not full rank, then dependent rows
  will be deleted first.
  """
  if verbose:
    print 'received H with size: ', H.shape

  # First, put the H matrix into the form H = [I|m] where:
  #   I is (n-k) x (n-k) identity matrix
  #   m is (n-k) x k
  # This part is just copying the algorithm from getSystematicGmatrix
  tempArray = getSystematicGmatrix(H)

  # Next, swap I and m columns so the matrix takes the forms [m|I].
  n      = H.shape[1]
  k      = n - H.shape[0]
  I_temp = tempArray[:,0:(n-k)]
  m      = tempArray[:,(n-k):n]
  newH   = concatenate((m,I_temp),axis=1)

  # Now the submatrix m is the transpose of the parity submatrix,
  # i.e. H is in the form H = [P'|I]. So G is just [I|P]
  k = m.shape[1]
  G = concatenate((identity(k),m.T),axis=1)
  if verbose:
    print 'returning G with size: ', G.shape
  return G